#include "platform.h"
#include "os.h"
#include "cgdoom.h"
#include <stdlib.h>
#include "libprof.h"

#include "doomtype.h"
#include "m_misc.h"
#include "d_main.h"

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

unsigned short *VRAM;
unsigned char *SaveVRAMBuffer;
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

/* Index of most likely ROM sectors. */
typedef struct {
	const void *sector;
	uint32_t start_bytes;
} SectorIndexInfo;

/* Description of a fragment of a WAD file, in Flash. This is placed in PRAM0
   to save RAM elsewhere. PRAM0 only supports 32-bit accesses, so we make this
   structure a volatile bitfield where each entry has a 32-bit supporting type,
   and build with -fstrict-volatile-bitfields which instructs GCC to use
   accesses of the size of the supporting type without compromising on the
   naming of the fields. */
typedef volatile struct {
	/* Index of first sector in Flash (0...64k) */
	uint32_t flash_address :16;
	/* Corresponding sector in the WAD file */
	uint32_t file_address :16;
} FileMappingItem;

/* Maximum number of fragments in the file map. Fragment information is stored
   in PRAM0, so up to ~40k entries can be stored; this limit is mostly to catch
   failures in fragment detection. (I have witnessed Ultimate Doom WADs with
   up to 500 fragments.)*/
#define MAX_FRAGMENTS 2048

typedef struct {
	/* Table of fragments */
	FileMappingItem *mTable;
	int miItemCount;
	/* Length of the file */
	int miTotalLength;
} FileMapping;

/* WAD file access method. */
int CGD_WADMethod = CGDOOM_WAD_MMAP;
/* File descriptor to WAD, used in Flash_ReadFile calls. (CGDOOM_WAD_BFILE) */
static int gWADfd = -1;
/* Memory map of WAD file. (CGDOOM_WAD_MMAP) */
static FileMapping gWADMap;
/* Index of most likely sectors for fragment search. (CGDOOM_WAD_MMAP) */
static SectorIndexInfo *gIndex = NULL;

/* Global options */
int CGD_TrustUnalignedLumps = 1;
int CGD_EnableDemos = 0;
int CGD_SingleEpisodeUltimate = 0;
int CGD_2MBLineMemory = 0;
int CGD_Frameskip = 1;
const char *CGD_WADFileName = NULL;

/* Delayed file accesses */
CGD_DelayedFileWrite CGD_DelayedSaves[6] = { 0 };

/* Performance counters */
struct CGD_Perf CGD_Perf;
/* Developer statistics */
struct CGD_Stats CGD_Stats;

#ifndef CG_EMULATOR

//The whole sound doesn't fir onto the RAM.
//Reading per partes is not possible as this is synchronnous player (there would be silences when reading).

/* Fast memcmp() for 512-byte sectors. */
int CGD_sector_memcmp(const void *fast_ram, const void *rom, size_t _512);

/* Caching structure to read WAD files by larger chunks than Flash sectors. */
typedef struct {
	int fd;
	int size, offset;
	char *data; /* of size FLASH_BFILE_UNIT */
} FileAccessCache;

/* Read next sector from file, while caching into a buffer. */
const void *ReadNextSector(FileAccessCache *fc, int *size)
{
	if(fc->size == 0)
	{
		fc->size = Bfile_ReadFile_OS(fc->fd, fc->data, FLASH_BFILE_UNIT, -1);
		fc->offset = 0;
	}
	if(fc->size <= 0)
	{
		*size = -1;
		return NULL;
	}

	*size = (fc->size < FLASH_PAGE_SIZE) ? fc->size : FLASH_PAGE_SIZE;
	fc->size -= *size;
	const void *sector = fc->data + fc->offset;
	fc->offset += *size;
	return sector;
}

/* Compare two sectors in ROM for the index. */
int IndexCompareSectors(const void *p1, const void *p2)
{
	const SectorIndexInfo *i1 = p1;
	const SectorIndexInfo *i2 = p2;
	return i1->start_bytes - i2->start_bytes;
}

/* Find all matching sectors in index (returns in-out interval). */
void IndexSearchSector(SectorIndexInfo *index, const void *buf, int *lo_ptr, int *hi_ptr)
{
	uint32_t needle = *(const uint32_t *)buf;
	*lo_ptr = *hi_ptr = -1;

	/* Find the first occurrence, set it in *lo_ptr */
	int lo=0, hi=FLASH_INDEX_SIZE;

	while(lo < hi) {
		int m = (lo + hi) / 2;
		if(index[m].start_bytes < needle) lo = m + 1;
		else hi = m;
	}

	if(lo >= FLASH_INDEX_SIZE || index[lo].start_bytes != needle) return;
	*lo_ptr = hi = lo;

	/* Store last occurrence in *hi_ptr */
	do hi++;
	while(hi < FLASH_INDEX_SIZE && index[hi].start_bytes == needle);

	*hi_ptr = hi;
}

/* Find a flash sector which contains the same data as buf. */
int FindSectorInFlash(const void *buf, int size)
{
	typeof(&memcmp) memcmp_fun = &memcmp;
	if(size == FLASH_PAGE_SIZE) memcmp_fun = &CGD_sector_memcmp;

	CGD_Stats.WADFragments++;

#ifdef FLASH_INDEX
	/* If an index has been built, search in it */
	int lo, hi;
	IndexSearchSector(gIndex, buf, &lo, &hi);
	for(int i = lo; i < hi; i++) {
		if(!memcmp_fun(buf, gIndex[i].sector, size)) {
			CGD_Stats.WADIndexHits++;
			return (gIndex[i].sector - FLASH_START) / FLASH_PAGE_SIZE;
		}
	}
#endif

	const void *sector = FLASH_FS_HINT;
	do {
		if(!memcmp_fun(buf, sector, size))
			return (sector - FLASH_START) / FLASH_PAGE_SIZE;

		sector += FLASH_PAGE_SIZE;
		if(sector == FLASH_END)
			sector = FLASH_START;
	}
	while(sector != FLASH_FS_HINT);
	return -1;
}

int CreateFileMapping(int fd, FileMapping *pMap)
{
	/* Cache accesses through a larger buffer */
	FileAccessCache fc = {
		.data = (void *)0xe5007000, /* XRAM */
		.fd = fd
	};
	int iLength = 0;
	int iFileSize = Bfile_GetFileSize_OS(fd);

	pMap->miItemCount = 0;
	pMap->miTotalLength = 0;

	int iLastProgress = 0;

	const void *pFileData = ReadNextSector(&fc, &iLength);
	while(iLength > 0)
	{
		/* Don't show this too often, or it will eat several seconds */
		if((pMap->miTotalLength - iLastProgress) * 20 > iFileSize) {
			UI_ProgressBar(HEIGHT-10, pMap->miTotalLength, iFileSize);
			iLastProgress = pMap->miTotalLength;
		}

		int iSectorID = FindSectorInFlash(pFileData, iLength);
		if(iSectorID == -1)
			return -2; // Page not found!

		pMap->miItemCount++;
		if(pMap->miItemCount >= MAX_FRAGMENTS)
			return -3; // File too fragmented!

		pMap->mTable[pMap->miItemCount-1].flash_address = iSectorID;
		pMap->mTable[pMap->miItemCount-1].file_address = pMap->miTotalLength / FLASH_PAGE_SIZE;

		/* Look for consecutive sectors in the same fragment */
		const void *pFragment = FLASH_START + (iSectorID * FLASH_PAGE_SIZE);
		if(!CGD_Stats.WADLowestFragment || pFragment<CGD_Stats.WADLowestFragment)
			CGD_Stats.WADLowestFragment = pFragment;

		for(;;)
		{
			pMap->miTotalLength += iLength;
			iSectorID++;
			pFragment += FLASH_PAGE_SIZE;

			if(iLength < FLASH_PAGE_SIZE)
			{
				//this was the last page
				return pMap->miTotalLength;
			}
			pFileData = ReadNextSector(&fc, &iLength);
			if(iLength <= 0)
				break;

			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wstringop-overread"
			if((iLength == FLASH_PAGE_SIZE)
				? CGD_sector_memcmp(pFileData, pFragment, iLength)
				: memcmp(pFileData, pFragment, iLength))
					break;
			#pragma GCC diagnostic pop
		}
	}
	if(iLength < 0)
		return -1;
	return pMap->miTotalLength;
}

/* Find a fragment in the file map by binary search. */
int FindFragmentInMap(FileMapping *map, int sector_number)
{
	int lo=0, hi=map->miItemCount;

	while (lo < hi)
	{
		int m = (lo + hi) / 2;
		if (map->mTable[m].file_address > sector_number) hi = m;
		else lo = m + 1;
	}

	return hi - 1;
}

int FindInFlash(const void **buf, int size, int readpos)
{
	if(CGD_WADMethod == CGDOOM_WAD_BFILE)
		return 0;

	ASSERT(readpos >= 0);
	if (readpos + size > gWADMap.miTotalLength)
		return -1;

	int iFragIndx = FindFragmentInMap(&gWADMap, readpos / FLASH_PAGE_SIZE);
	int iFragOffset = gWADMap.mTable[iFragIndx].file_address * FLASH_PAGE_SIZE;
	int iSubOffset = readpos - iFragOffset;

	int iFragEnd;
	if (iFragIndx+1 < gWADMap.miItemCount)
		iFragEnd = gWADMap.mTable[iFragIndx+1].file_address * FLASH_PAGE_SIZE;
	else
		iFragEnd = gWADMap.miTotalLength;

	*buf = FLASH_CACHED_START + (gWADMap.mTable[iFragIndx].flash_address * FLASH_PAGE_SIZE) + iSubOffset;

	/* Return how many bytes can be read off the fragment (up to size). */
	int iAvailableLen = (iFragEnd-readpos < size) ? iFragEnd-readpos : size;

	ASSERT(iAvailableLen > 0);
	return iAvailableLen;
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
	if(CGD_WADMethod == CGDOOM_WAD_BFILE)
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

/* Find WAD files in the filesystem. */
int FindWADs(CGD_WADFileInfo *files, int max)
{
	uint16_t path[32];
	Bfile_FileInfo info;

	int sd, rc, total=0;
	rc = Bfile_FindFirst(u"\\\\fls0\\*.wad", &sd, path, &info);

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
	CGD_DelayedFileWrite const *dfw = &CGD_DelayedSaves[i];

    uint16_t fc_path[100] = u"\\\\fls0\\";
    int j=7, rc, fd;

    for (int i = 0; dfw->filename[i]; i++)
        fc_path[j++] = dfw->filename[i];
    fc_path[j++] = 0x0000;

    UI_DelayedWrites(CGD_DelayedSaves, 6, i, -1);

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

        UI_DelayedWrites(CGD_DelayedSaves, 6, i, dfw->size - size);
    }

    Bfile_CloseFile_OS(fd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	extern boolean autostart;
	int autostart_ = 0;
	startmap = 1;
	startepisode = 1;

	VRAM = (unsigned short*)GetVRAMAddress();

	int time, ms_index=0, ms_mmap=0;

	EnableColor(1);
	EnableStatusArea(3);
	Bdisp_FrameAndColor(3, 16);
	Bdisp_FrameAndColor(1, 0);

	prof_init();

	CGD_WADFileInfo wads[16];
	int wad_count = FindWADs(wads, 16);

	int dev_info = 0;

	/* Allow the user to use memory past the 2 MB line on known OS versions */
	int *enable_2MBline=NULL;
	char const *osv = GetOSVersion();
	if(!strncmp(osv, "03.", 3) && osv[3] <= '6') // 3.60 or earlier
		enable_2MBline = &CGD_2MBLineMemory;

	/* Load initial keyboard layout */
	CGD_LoadKeymap(CGD_Keymap_ThumbsOnly);

	int choice = UI_Main(wads, wad_count, &dev_info, &CGD_WADMethod,
		&startmap, &startepisode, &CGD_TrustUnalignedLumps, &autostart_,
		&CGD_EnableDemos, enable_2MBline);
	if(choice < 0)
		return 1;

	autostart = autostart_;

	/* Parameters unavailable on the SDL2 build */
	#ifdef CG_EMULATOR
	CGD_WADMethod = CGDOOM_WAD_BFILE;
	CGD_TrustUnalignedLumps = 0;
	CGD_2MBLineMemory = 0;
	#endif

	/* Override version detection for single-episode Ultimate Doom WADs */
	if (!strcmp(wads[choice].name, "doomu1.wad"))
		CGD_SingleEpisodeUltimate = 1;
	if (!strcmp(wads[choice].name, "doomu2.wad"))
		CGD_SingleEpisodeUltimate = 2;
	if (!strcmp(wads[choice].name, "doomu3.wad"))
		CGD_SingleEpisodeUltimate = 3;
	if (!strcmp(wads[choice].name, "doomu4.wad"))
		CGD_SingleEpisodeUltimate = 4;

	uintptr_t secondary_vram = ((uintptr_t)GetSecondaryVRAMAddress() + 3) & -4;
	SaveVRAMBuffer = (void *)secondary_vram;

	/* fx-CG 50 / Graph 90+E: RAM starts at 0x0c000000 in physical memory */
	SystemStack = (void *)0xac0f0000;

	/* Determine how much RAM is zeroed out at 2 MB */
	if(CGD_2MBLineMemory)
		CGD_2MBLineMemory = FindZeroedMemory((void *)0xac200000);

	void *PRAM0_alloc_start = PRAM0_START;

	/* Remember WAD file name for saves and loads */
	static char wad_name[32] = { 0 };
	for (int i = 0; wads[choice].name[i] != '.'; i++)
		wad_name[i] = wads[choice].name[i];
	CGD_WADFileName = wad_name;

	/* Setup access to WAD file */
	if(CGD_WADMethod == CGDOOM_WAD_BFILE)
	{
		gWADfd = Bfile_OpenFile_OS(wads[choice].path, 0, 0);
	}
	else
	{
		#ifdef FLASH_INDEX
		/* Index most likely flash sectors into a sorted array, so that
		   sectors can be hit quickly. The index contains every sector
		   on a 4-kiB boundary (where fragments are most likely to
		   start) between FLASH_FS_HINT and FLASH_END. */
		time = RTC_GetTicks();
		gIndex = (void *)SystemStack;
		for(int i = 0; i < FLASH_INDEX_SIZE; i++) {
			SectorIndexInfo *info = &gIndex[i];
			info->sector = FLASH_FS_HINT + (i * 4096);
			info->start_bytes = *(const uint32_t *)info->sector;
		}
		qsort(gIndex, FLASH_INDEX_SIZE, sizeof *gIndex, IndexCompareSectors);
		ms_index = (RTC_GetTicks() - time) * 8;
		#endif /* FLASH_INDEX */

		time = RTC_GetTicks();
		gWADMap.mTable = PRAM0_START;
		int fd = Bfile_OpenFile_OS(wads[choice].path, 0, 0);
		int size = CreateFileMapping(fd, &gWADMap);
		Bfile_CloseFile_OS(fd);
		UI_ProgressBar(HEIGHT-10, 1, 1);
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
		else if(dev_info) {
			Layout l;
			Layout_Init(&l);
			Bdisp_AllClr_VRAM();
			Layout_CenteredText(&l, "Developer info");
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "Fragments:", "%d", CGD_Stats.WADFragments);
			Layout_Text(&l, "Index hits:", "%d (%d%%)", CGD_Stats.WADIndexHits,
				(CGD_Stats.WADIndexHits * 100 / CGD_Stats.WADFragments));
			Layout_Text(&l, "Lowest fragment:", "%p",
				CGD_Stats.WADLowestFragment);
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "Index build time:", "%d ms", ms_index);
			Layout_Text(&l, "File mapping time:", "%d ms", ms_mmap);
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "Memory beyond the 2MB line:", "%d kB",
				CGD_2MBLineMemory >> 10);
			Bdisp_PutDisp_DD();
			int key;
			GetKey(&key);
		}

		PRAM0_alloc_start += gWADMap.miItemCount * sizeof(FileMappingItem);
	}

	/* Initialize the PRAM allocator */
	CGD_PRAM_Init(PRAM0_alloc_start, PRAM0_END);

	memset(VRAM, 0, WIDTH*HEIGHT*2);
	D_DoomMain();

	if(gWADfd >= 0)
		Bfile_CloseFile_OS(gWADfd);

	int delayed_writes = sizeof(CGD_DelayedSaves) / sizeof(CGD_DelayedSaves[0]);
	for(int i = 0; i < delayed_writes; i++) {
		CGD_DelayedFileWrite *dfw = &CGD_DelayedSaves[i];
		if(dfw->data != NULL)
			DelayedWriteFile(i);
	}

	if(dev_info) {
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
