#include "platform.h"
#include "os.h"
#include "cgdoom.h"
#include <stdlib.h>
#include "libprof.h"

#include "doomtype.h"
#include "m_misc.h"
#include "d_main.h"
#include "v_video.h"
#include "g_game.h"

#include "cgdoom-alloc.h"
#include "cgdoom-kbd.h"
#include "cgdoom-ui.h"

void *CGD_malloc(int size)
{
	void *p = malloc(size);
	if (!p)
		I_Error ("CGD_malloc(%d) failure", size);
	return p;
}

void *CGD_calloc(int size)
{
	void *p = CGD_malloc(size);
	if(p)
		memset(p, 0, size);
	return p;
}

void *CGD_realloc(void *p, int size)
{
	if(p == NULL)
		return malloc(size);
	else
		return realloc(p, size);
}

uint16_t *VRAM;
uint16_t *SecondaryVRAM;
unsigned char *SystemStack;

int strnicmp(const char *s1,const char *s2,int count)
{
	for(int i = 0; i < count; i++) {
		int c1 = (s1[i] >= 'a' && s1[i] <= 'z') ? s1[i] & ~0x20 : s1[i];
		int c2 = (s2[i] >= 'a' && s2[i] <= 'z') ? s2[i] & ~0x20 : s2[i];

		if(c1 != c2 || !c1 || !c2)
			return c1 - c2;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// WAD file access mechanism
//
// Since BFile is too slow to access the WAD file in real-time, the fragments
// are searched in ROM and their physical addresses are stored for direct
// access later. This is similar to mmap() except that a manual translation is
// used instead of the MMU. As far as I know, the first version of this system
// was implemented by Martin Poupe.
//
// The file is obviously fragmented and Yatis reverse-engineered Fugue enough
// to determine that storage units are sectors of 512 bytes. While clusters of
// 4 kiB are used in general, a file might not start on the first sector of a
// cluster, and some sectors might also simply be dead.
//
// Although all 65536 Flash sectors are searched when needed, several
// heuristics are used:
// * The region between FLASH_FS_HINT and FLASH_END is searched first, since
//   this is roughly where the filesystem is located.
// * All sectors aligned on 4-kiB boundaries between FLASH_FS_HINT and
//   FLASH_END (of which there are 4096) are indexed by their first 4 bytes and
//   binary searched for matches before anything else.
//
// See <src-cg/platform.h> for Flash traversal parameters.
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/* Table of file blocks */
	const void *blocks[WAD_MAX_BLOCKS];
	int miItemCount;
	/* Length of the file */
	int miTotalLength;
} FileMapping;

/* File descriptor to WAD, used in Flash_ReadFile calls. (CGDOOM_WAD_BFILE) */
static int gWADfd = -1;
/* Memory map of WAD file. (CGDOOM_WAD_MMAP) */
static FileMapping gWADMap;
CGDoom_Options CGD_Options;
CGDoom_Perf CGD_Perf;
CGDoom_Stats CGD_Stats;

/* Global options */
int CGD_SingleEpisodeUltimate = 0;
int CGD_2MBLineMemory = 0;
int CGD_Frameskip = 1;
const char *CGD_WADFileName = NULL;
int CGD_RecordDemoSlot = -1;
int CGD_PlayDemoOnly = 0;

/* Delayed file accesses */
CGDoom_DelayedFileWrite CGD_DelayedSaves[CGD_DELAYEDSAVES_COUNT] = { 0 };

#ifndef CG_EMULATOR

int CreateFileMapping(int fd, FileMapping *pMap)
{
	int iFileSize = Bfile_GetFileSize_OS(fd);

	pMap->miItemCount = 0;
	pMap->miTotalLength = iFileSize;

	if(iFileSize <= 0 || iFileSize > WAD_MAX_SIZE)
		return -1;

	pMap->miItemCount =
		(iFileSize + WAD_BLOCK_SIZE - 1) / WAD_BLOCK_SIZE;

	for(int i = 0; i < pMap->miItemCount; i++) {
		void *uncached;
		int rc = Bfile_GetBlockAddress(fd, i * WAD_BLOCK_SIZE, &uncached);
		if(rc < 0 || !uncached)
			return -2;

		uintptr_t address = (uintptr_t)uncached;
		if(address < (uintptr_t)FLASH_START ||
		   address > (uintptr_t)FLASH_END - WAD_BLOCK_SIZE)
			return -2;

		pMap->blocks[i] = (const void *)(
			(uintptr_t)FLASH_CACHED_START + address -
			(uintptr_t)FLASH_START);
	}

	CGD_Stats.WADFragments = pMap->miItemCount;
	return iFileSize;
}

int FindInFlash(const void **buf, int size, int readpos)
{
	if(CGD_Options.WADMethod == CGDOOM_WAD_BFILE)
		return 0;

	if(readpos < 0 || size < 0 ||
	   readpos > gWADMap.miTotalLength ||
	   readpos + size > gWADMap.miTotalLength)
		return -1;

	int iBlockCount = readpos / WAD_BLOCK_SIZE;
	int iBlockOffset = readpos % WAD_BLOCK_SIZE;
	int iAvailableLen = WAD_BLOCK_SIZE - iBlockOffset;

	if(iBlockCount >= gWADMap.miItemCount)
		return -1;

	*buf = (const char *)gWADMap.blocks[iBlockCount] + iBlockOffset;
	return iAvailableLen < size ? iAvailableLen : size;
}

static int FindZeroedMemory(void *start)
{
	/* Look for zero-longwords every 16 bytes */
	int size = 0;

	/* Limit to 6 MB since the fx-CG 50 doesn't have any more memory and
	   anything after the RAM is likely to be zero non-writable */
	while(size < CGDOOM_2MBLINEMEMORY_MAX && !*(uint32_t *)(start + size))
		size += 16;

	/* Round down to a multiple of 4096 */
	return size & ~0xfff;
}

__attribute__((used, noinline, noreturn))
void abort(void){
	int x=0,y=160;
	PrintMini(&x,&y,"Abort called",0,0xFFFFFFFF,0,0,0xFFFF,0,1,0);
	int key;
	for(;;)
		GetKey(&key);
}

#endif /* CG_EMULATOR */

int Flash_ReadFile(void *buf, int size, int readpos)
{
	if(CGD_Options.WADMethod == CGDOOM_WAD_BFILE)
		return Bfile_ReadFile_OS(gWADfd, buf, size, readpos);

	const void *pSrc;
	int iRet = 0;
	while(size >0)
	{
		int i = FindInFlash(&pSrc,size, readpos);
		if(i<0) {
			I_Error ("Flash_ReadFile: cannot find position %d (size %d)", readpos,
				size);
			return i;
		}
		memcpy(buf,pSrc,i);
		buf = ((char*)buf)+i;
		readpos +=i;
		size -=i;
		iRet +=i;
	}
	return iRet;
}

/* Find files in the filesystem based on a pattern. */
static int FindFiles(const uint16_t *wildcard, CGDoom_FileInfo *files, int max)
{
	uint16_t path[32];
	Bfile_FileInfo info;

	int sd, rc, total=0;
	rc = Bfile_FindFirst(wildcard, &sd, path, &info);

	while(rc != -16 && total < max)
	{
		memcpy(files[total].path, u"\\\\fls0\\", 14);
		memcpy(files[total].path+7, path, 32*2);
		Bfile_NameToStr_ncpy(files[total].name, path, 32);
		files[total].size = info.fsize;
		total++;

		rc = Bfile_FindNext(sd, path, &info);
	}

	Bfile_FindClose(sd);
	return total;
}

static void DelayedWriteFile(int i)
{
	CGDoom_DelayedFileWrite const *dfw = &CGD_DelayedSaves[i];

    uint16_t fc_path[100] = u"\\\\fls0\\";
    int j=7, rc, fd;

    for (int i = 0; dfw->filename[i]; i++)
        fc_path[j++] = dfw->filename[i];
    fc_path[j++] = 0x0000;

    UI_DelayedWrites(CGD_DelayedSaves, CGD_DELAYEDSAVES_COUNT, i, -1);

    Bfile_DeleteEntry(fc_path);

    rc = Bfile_CreateEntry_OS(fc_path, BFILE_CREATEMODE_FILE,
        (size_t *)&dfw->size);
    if (rc < 0) {
        I_Error("Bfile_CreateEntry_OS(%s, %d bytes): %d", dfw->filename,
            dfw->size, rc);
        return;
    }

    fd = Bfile_OpenFile_OS(fc_path, BFILE_WRITE, 0);
    if (fd < 0) {
        I_Error("Bfile_OpenFile_OS(%s): %d", dfw->filename, fd);
        return;
    }

    /* Write chunks of 4096 bytes and show progress in-between chunks */
    int size = dfw->size;
    const void *source = dfw->data;

    while(size > 0) {
        int chunk_size = (size >= 4096) ? 4096 : size;
        Bfile_WriteFile_OS(fd, source, chunk_size);
        source += chunk_size;
        size -= chunk_size;

        UI_DelayedWrites(CGD_DelayedSaves, CGD_DELAYEDSAVES_COUNT, i,
            dfw->size - size);
    }

    Bfile_CloseFile_OS(fd);
}

//---
// Setting save/load
//---

static void SaveSettings(const CGDoom_Options *o)
{
	char *out_start = (char *)CGDOOM_SCREENS_BASE;
	char *out = out_start;

	/* Version identifcation (just in case something goes wrong) */
	out += sprintf(out, "Version=%d\n", CGDOOM_SETTINGS_FILE_VERSION);

	/* Settings */
	out += sprintf(out, "WADMethod=%s\n",
		o->WADMethod == CGDOOM_WAD_BFILE ? "BFILE" :
		o->WADMethod == CGDOOM_WAD_MMAP  ? "MMAP"  :
		"?");
	out += sprintf(out, "DeveloperInfo=%d\n",
		o->DeveloperInfo);
	out += sprintf(out, "TrustUnalignedLumps=%d\n",
		o->TrustUnalignedLumps);
	out += sprintf(out, "EnableDemos=%d\n",
		o->EnableDemos);
	out += sprintf(out, "Autostart=%d\n",
		o->AutoStart);
	out += sprintf(out, "ExperimentalMemory=%d\n",
		o->EnableExperimentalMemory);

	/* Keyboard layout */
	for(int i = 0; i < CGDOOM_KEYMAP_SIZE; i++) {
		out += sprintf(out, "Keymap.%s=%s\n",
			CGD_DoomKey_TechnicalName(CGD_KeymapEntries[i]),
			CGD_PhysicalKey_TechnicalName(o->Keymap[i]));
	}

	M_WriteFile("CGDoom.cfg", out_start, out - out_start);
	memset(out_start, 0xff, out - out_start);
}

static void LoadSettings(CGDoom_Options *o)
{
	char *buffer = (char *)CGDOOM_SCREENS_BASE;
	int length = M_ReadFile("CGDoom.cfg", (byte **)&buffer, 0);
	if(length <= 0)
		return;

	/* Parse every line on the form "<Key>=<Value>\n" */
	const char *line, *nextline=NULL;

	for(line = buffer; line < buffer + length; line = nextline) {
		/* Start of next line, or end of string */
		nextline = strchr(line, '\n');
		nextline = nextline ? nextline + 1 : line + strlen(line);

		if(line[0] == '#' || line[0] == '\n')
			continue;

		const char *eq = strchr(line, '=');
		const char *value = eq+1;
		if(!eq || eq >= nextline)
			continue;

		if(!strncmp(line, "Version", eq-line)) {
			int version = atoi(value);
			if(version != CGDOOM_SETTINGS_FILE_VERSION)
				return;
		}

		else if(!strncmp(line, "WADMethod", eq-line)) {
			if(!strncmp(value, "BFILE", 5))
				o->WADMethod = CGDOOM_WAD_BFILE;
			else if(!strncmp(value, "MMAP", 4))
				o->WADMethod = CGDOOM_WAD_MMAP;
		}

		else if(!strncmp(line, "Keymap.", 7)) {
			int k1 = CGD_DoomKey_FromTechnicalName(line+7, eq-line-7);
			int k2 = CGD_PhysicalKey_FromTechnicalName(value,nextline-value-1);

			if(k1 > 0 && k2 >= 0) {
				for(int i = 0; i < CGDOOM_KEYMAP_SIZE; i++) {
					if(CGD_KeymapEntries[i] == k1) {
						o->Keymap[i] = k2;
						break;
					}
				}
			}
		}

		else if(!strncmp(line, "DeveloperInfo", eq-line))
			o->DeveloperInfo = atoi(value);
		else if(!strncmp(line, "TrustUnalignedLumps", eq-line))
			o->TrustUnalignedLumps = atoi(value);
		else if(!strncmp(line, "EnableDemos", eq-line))
			o->EnableDemos = atoi(value);
		else if(!strncmp(line, "Autostart", eq-line))
			o->AutoStart = atoi(value);
		else if(!strncmp(line, "ExperimentalMemory", eq-line))
			o->EnableExperimentalMemory = atoi(value);
	}
}

static int SettingsChanged(const CGDoom_Options *before,
	const CGDoom_Options *after)
{
	return memcmp(before, after, sizeof *before) != 0;
}

//---
// Main function
//---

int main(void)
{
	//---
	// First configuration step
	//---

	VRAM = (void *)GetVRAMAddress();

    uintptr_t vram2 = ((uintptr_t)GetSecondaryVRAMAddress() + 3) & -4;
    SecondaryVRAM = (void *)vram2;

	EnableColor(1);
	EnableStatusArea(3);
	Bdisp_FrameAndColor(3, 16);
	Bdisp_FrameAndColor(1, 0);

	prof_init();

	/* Allow the user to use memory past the 2 MB line on tested OS versions */
	int allow_experimental_RAM = 0;
	char const *osv = GetOSVersion();
	if(!strncmp(osv, "03.", 3) && osv[3] <= '6') // 3.60 or earlier
		allow_experimental_RAM = 1;
	else
		CGD_Options.EnableExperimentalMemory = 0;

	//---
	// Load default and saved options
	//---

	CGDoom_Options o = {
		.DeveloperInfo				= 0,
		.WADMethod					= CGDOOM_WAD_MMAP,
		.TrustUnalignedLumps		= 1,
		.AutoStart					= 0,
		.EnableDemos				= 0,
		.EnableExperimentalMemory	= 0,
	};
	CGD_CopyKeymap(o.Keymap, CGD_Keymap_ThumbsOnly);

	/* Override with settings read from file if there is one */
	LoadSettings(&o);

	/* Keep o so we can later look for changes and resave them */
	memcpy(&CGD_Options, &o, sizeof o);

    /* Offer to start at E1M1 if skipping the title screen */
	extern int startmap;
	extern int startepisode;
	startmap = 1;
	startepisode = 1;

	//---
	// Run main menu and apply settings
	//---

	/* Look for WAD files at the root of the filesystem */
	CGDoom_FileInfo wads[16];
	int wad_count = FindFiles(u"\\\\fls0\\*.wad", wads, 16);

	/* Look for demo files to replay, also at the root */
	CGDoom_FileInfo demos[32];
	int demo_count = FindFiles(u"\\\\fls0\\*_demo*.lmp", demos, 32);

	CGDoom_FileInfo *selection = UI_Main(wads, wad_count, demos, demo_count,
		&CGD_Options, &startmap, &startepisode, &CGD_RecordDemoSlot,
		allow_experimental_RAM);

	int choice = -1;
	if(selection >= demos && selection < demos + demo_count) {
		/* Find the WAD file with the correct name */
		for(int i = 0; i < wad_count; i++) {
			int len = strlen(wads[i].name) - 4; /* omit ".wad" */
			if(!strncmp(selection->name, wads[i].name, len)
					&& !strncmp(selection->name + len, "_demo", 5)) {
				choice = i;
				break;
			}
		}
		if(choice == -1) {
			UI_Error("The WAD file for %s has been removed or renamed!",
				selection->name);
			return 1;
		}
		G_DeferedPlayDemoFile(selection->name);
		CGD_PlayDemoOnly = 1;
	}
	else if(selection >= wads && selection < wads + wad_count) {
		choice = selection - wads;
	}
	else return 1;

	/* Override parameters unavailable on the SDL2 build */
	#ifdef CG_EMULATOR
	CGD_Options.WADMethod = CGDOOM_WAD_BFILE;
	CGD_Options.TrustUnalignedLumps = 0;
	CGD_Options.EnableExperimentalMemory = 0;
	#endif

	/* Apply settings to Doom variables */
	extern boolean autostart;
	autostart = CGD_Options.AutoStart;

	/* Determine how much RAM is zeroed out at 2 MB */
	if(CGD_Options.EnableExperimentalMemory)
		CGD_2MBLineMemory = FindZeroedMemory((void *)0xac200000);
	else
		CGD_2MBLineMemory = 0;

	/* Save settings for the next load */
	if(SettingsChanged(&o, &CGD_Options))
		SaveSettings(&CGD_Options);

	//---
	// Second configuration step
	//---

	void *PRAM0_alloc_start = PRAM0_START;

	/* Override version detection for single-episode Ultimate Doom WADs */
	if (!strcmp(wads[choice].name, "doomu1.wad"))
		CGD_SingleEpisodeUltimate = 1;
	if (!strcmp(wads[choice].name, "doomu2.wad"))
		CGD_SingleEpisodeUltimate = 2;
	if (!strcmp(wads[choice].name, "doomu3.wad"))
		CGD_SingleEpisodeUltimate = 3;
	if (!strcmp(wads[choice].name, "doomu4.wad"))
		CGD_SingleEpisodeUltimate = 4;

	/* Remember WAD file name for saves and loads */
	static char wad_name[32] = { 0 };
	for (int i = 0; wads[choice].name[i] != '.'; i++)
		wad_name[i] = wads[choice].name[i];
	CGD_WADFileName = wad_name;

	/* fx-CG 50 / Graph 90+E: RAM starts at 0x0c000000 in physical memory */
	SystemStack = (void *)0xac0f0000;

	//---
	// Setup access to the WAD file
	//---

	int time, ms_mmap=0;

	if(CGD_Options.WADMethod == CGDOOM_WAD_BFILE)
	{
		gWADfd = Bfile_OpenFile_OS(wads[choice].path, 0, 0);
	}
	else
	{
		time = RTC_GetTicks();
		int fd = Bfile_OpenFile_OS(wads[choice].path, 0, 0);
		int size = CreateFileMapping(fd, &gWADMap);
		Bfile_CloseFile_OS(fd);
		// UI_ProgressBar(HEIGHT-10, 1, 1);
		ms_mmap = (RTC_GetTicks() - time) * 8;

		if(size == -1) {
			I_Error ("File read error");
			return 1;
		}
		else if(size == -2) {
			I_Error ("Page not found");
			return 1;
		}
		else if(size == -3) {
			I_Error ("File too fragmented");
			return 1;
		}
		else if(CGD_Options.DeveloperInfo) {
			Layout l;
			Layout_Init(&l);
			Bdisp_AllClr_VRAM();
			Layout_CenteredText(&l, "Developer info");
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "Fragments:", "%d", CGD_Stats.WADFragments);
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "File mapping time:", "%d ms", ms_mmap);
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "Memory beyond the 2MB line:", "%d kB",
				CGD_2MBLineMemory >> 10);
			Bdisp_PutDisp_DD();
			GetKey(&(int){});
		}
	}

	/* Initialize the PRAM allocator */
	CGD_PRAM_Init(PRAM0_alloc_start, PRAM0_END);

	memset(VRAM, 0, WIDTH*HEIGHT*2);
	D_DoomMain();

	if(gWADfd >= 0)
		Bfile_CloseFile_OS(gWADfd);

	for(int i = 0; i < CGD_DELAYEDSAVES_COUNT; i++) {
		CGDoom_DelayedFileWrite *dfw = &CGD_DelayedSaves[i];
		if(dfw->data != NULL)
			DelayedWriteFile(i);
	}

	if(CGD_Options.DeveloperInfo) {
		Layout l;
		Layout_Init(&l);
		Bdisp_AllClr_VRAM();
		Layout_CenteredText(&l, "Developer info");
		Layout_Spacing(&l, 12);

		Layout_Text(&l, "Memory allocated:", "%d kB",
			CGD_Stats.MemoryAllocated >> 10);
		Layout_Spacing(&l, 12);

		Layout_Text(&l, "Lumps loaded:", "%d (%d kB)",
			CGD_Stats.LumpsLoaded,
			(int)(CGD_Stats.LumpsLoadedTotal >> 10));
		Layout_Text(&l, "... of which unaligned:", "%d (%d kB)",
			CGD_Stats.UnalignedLumpsLoaded,
			(int)(CGD_Stats.UnalignedLumpsLoadedTotal >> 10));
		Layout_Text(&l, "Lumps referenced:", "%d (%d kB)",
			CGD_Stats.LumpsReferenced,
			(int)(CGD_Stats.LumpsReferencedTotal >> 10));
		Bdisp_FrameAndColor(3, 16);
		Bdisp_FrameAndColor(1, 0);
		Bdisp_PutDisp_DD();
		while(PRGM_GetKey() != 0) {}
		int key;
		GetKey(&key);
	}

	prof_quit();
	return EXIT_SUCCESS;
}
