#include "platform.h"
#include "os.h"
#include "cgdoom-ui.h"

#ifdef CG_EMULATOR
static int iAllocSum = 0;
#endif 
void * CGDMalloc(int iSize)
{
#ifdef CG_EMULATOR

	iAllocSum += iSize;
	printf("malloc %i (%i)\n",iSize,iAllocSum);
#endif 
	return malloc(iSize);
}

void * CGDCalloc(int iSize)
{
	void *p = CGDMalloc(iSize);
	if(p != NULL)
	{
		memset(p,0,iSize);
	}
	return p;
}


void * CGDRealloc (void *p, int iSize)
{
#ifdef CG_EMULATOR
	iAllocSum += iSize;
	printf("realloc %i (%i)\n",iSize,iAllocSum);
#endif 
	if(p == NULL)
	{
		return malloc(iSize);
	}
	else
	{
		return realloc(p,iSize);
	}
}


#ifdef CG_EMULATOR
unsigned char aSaveVRAMBuffer[SAVE_VRAM_SIZE]; //for screens[2] (2x64KB) + WAD file mapping
unsigned char aSystemStack[SYSTEM_STACK_SIZE]; //for RAM_I_Zone
#endif 
unsigned short *VRAM;
unsigned char *SaveVRAMBuffer; //for screens[2]
unsigned char *SystemStack; //for RAM_I_Zone


void CGDAppendNum09(const char *pszText,int iNum,char *pszBuf)
{
	int i = 0;
	while(pszText[i])
	{
		pszBuf[i] = pszText[i];
		i++;
	}
	ASSERT(iNum < 10);
	ASSERT(iNum >= 0);
	pszBuf[i] = (char)('0'+iNum);
	pszBuf[i+1] = 0;
}

void CGDAppendNum0_999(const char *pszText,int iNum,int iMinDigits,char *pszBuf)
{
	int i = 0;
	int z = 0;
	while(pszText[i])
	{
		pszBuf[i] = pszText[i];
		i++;
	}
	ASSERT(iNum < 1000000);
	ASSERT(iNum >= 0);
	if((iNum > 999999) || (iMinDigits>6))
	{
		pszBuf[i] = (char)('0'+(iNum / 1000000));
		iNum %= 1000000;
		i++;
		z = 1;
	}
	if(z || (iNum > 99990) || (iMinDigits>5))
	{
		pszBuf[i] = (char)('0'+(iNum / 100000));
		iNum %= 100000;
		i++;
		z = 1;
	}

	if(z || (iNum > 9999) || (iMinDigits>4))
	{
		pszBuf[i] = (char)('0'+(iNum / 10000));
		iNum %= 10000;
		i++;
		z = 1;
	}
	if(z || (iNum > 999) || (iMinDigits>3))
	{
		pszBuf[i] = (char)('0'+(iNum / 1000));
		iNum %= 1000;
		i++;
		z = 1;
	}
	if(z || (iNum > 99) || (iMinDigits>2))
	{
		pszBuf[i] = (char)('0'+(iNum / 100));
		iNum %= 100;
		i++;
		z = 1;
	}
	if(z || (iNum > 9) || (iMinDigits>1))
	{
		pszBuf[i] = (char)('0'+(iNum / 10));
		iNum %= 10;
		i++;
	}
	pszBuf[i] = (char)('0'+iNum);
	pszBuf[i+1] = 0;
}

void CGDAppendHex32(const char *pszText,int iNum,int iDigits,char *pszBuf)
{
	CGDstrcpy(pszBuf, pszText);
	pszBuf += CGDstrlen(pszText);
	for(int i = 0; i < iDigits; i++)
	{
		int c = (iNum >> (i * 4)) & 0xf;
		c = c + '0' + 7 * (c > 9);
		pszBuf[iDigits-i-1] = c;
	}
	pszBuf[iDigits] = 0;
}


int CGDstrlen(const char *pszText)
{
	int i = 0;
	while(pszText[i])
	{
		i++;
	}
	return i;
}

void CGDstrcpy(char *pszBuf,const char *pszText)
{
	int i = 0;
	while(pszText[i])
	{
		pszBuf[i] = pszText[i];
		i++;
	}
	pszBuf[i] = 0;
}

void CGDstrncpy(char *pszBuf,const char *pszText,int iLen)
{
	int i = 0;
	while(pszText[i])
	{
		if(iLen == i)
		{
			return;
		}
		pszBuf[i] = pszText[i];
		i++;
	}
	pszBuf[i] = 0;
}

int CGDstrcmp (const char*s1,const char*s2)
{
	while(s1[0])
	{
		unsigned char c1 = (unsigned char)s1[0];
		unsigned char c2 = (unsigned char)s2[0];
		if(c1 !=c2)
		{
			return 1;//who cares 1/-1, important is match/nonmatch
		}
		s1++;
		s2++;
	}
	return (unsigned char)s2[0]>0;
}

int CGDstrncmp (const char*s1,const char*s2,int iLen)
{
	if(!iLen)
	{
		return 0;
	}

	while(s1[0])
	{
		unsigned char c1 = (unsigned char)s1[0];
		unsigned char c2 = (unsigned char)s2[0];

		if(c1 !=c2)
		{
			return 1;//who cares 1/-1, important is match/nonmatch
		}
		s1++;
		s2++;

		iLen--;
		if(!iLen)
		{
			return 0;
		}
	}
	return (unsigned char)s2[0]>0;
}

static unsigned char Upper(unsigned char c)
{
	if((c>='a')&&(c<='z'))
	{
		c -='a'-'A';
	}
	return c;
}

int CGDstrnicmp (const char*s1,const char*s2,int iLen)
{
	if(!iLen)
	{
		return 0;
	}

	while(s1[0])
	{
		unsigned char c1 = Upper((unsigned char)s1[0]);
		unsigned char c2 = Upper((unsigned char)s2[0]);

		if(c1 !=c2)
		{
			return 1;//who cares 1/-1, important is match/nonmatch
		}
		s1++;
		s2++;
		iLen--;
		if(!iLen)
		{
			return 0;
		}
	}
	return (unsigned char)s2[0]>0;
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
// See <platform.h> for Flash traversal parameters.
///////////////////////////////////////////////////////////////////////////////

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

/* Index of most likely ROM sectors. */
typedef struct {
	const void *sector;
	uint32_t start_bytes;
} SectorIndexInfo;

//allocate 1024 items for max 1024 fragments of the file.
// 640 KB should to be enough for everyone ;-)
#define MAX_FRAGMENTS 1024

/* Description of a fragment of a WAD file, in Flash. */
typedef struct {
	/* Index of first sector in Flash (0...64k) */
	uint16_t flash_address;
	/* Corresponding sector in the WAD file */
	uint16_t file_address;
} FileMappingItem;


typedef struct
{
	FileMappingItem mTable[MAX_FRAGMENTS];//table of fragments
	int miItemCount;
	int miTotalLength;//length of the file
	int miCurrentLength;//currently returned length (by GetNextdata() )
	int miCurrentItem;//active fragment (to be returned by GetNextdata() )

}FileMapping;

/* Bfile's file info structure returned by filesystem search functions. */
typedef struct
{
	unsigned short id, type;
	unsigned long fsize, dsize;
	unsigned int property;
	unsigned long address;
} Bfile_FileInfo;

//reset reading to start
void ResetData(FileMapping *pMap)
{
	pMap->miCurrentItem = 0;
	pMap->miCurrentLength = 0;
}

/* Find WAD files in the filesystem. */
int FindWADs(WADFileInfo *files, int max)
{
	uint16_t path[16];
	Bfile_FileInfo info;

	int sd, rc, total=0;
	rc = Bfile_FindFirst(u"\\\\fls0\\*.wad", &sd, path, &info);

	while(rc != -16 && total < max)
	{
		memcpy(files[total].path, u"\\\\fls0\\", 14);
		memcpy(files[total].path+7, path, 16*2);
		Bfile_NameToStr_ncpy(files[total].name, path, 16);
		files[total].size = info.fsize;
		total++;

		rc = Bfile_FindNext(sd, path, &info);
	}

	Bfile_FindClose(sd);
	return total;
}

void I_Error (char *error, ...);

/* WAD file access method. */
static int gWADmethod = CGDOOM_WAD_MMAP;
/* File descriptor to WAD, used in Flash_ReadFile calls. (CGDOOM_WAD_BFILE) */
static int gWADfd = -1;
/* Memory map of WAD file. (CGDOOM_WAD_MMAP) */
static FileMapping *gpWADMap = 0;
/* Index of most likely sectors for fragment search. (CGDOOM_WAD_MMAP) */
static SectorIndexInfo *gIndex = NULL;

/* Developer info */
static int gDevFragments = 0;
static int gDevIndexHits = 0;
static const void *gDevLowestFragment = NULL;

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

	*size = min(fc->size, FLASH_PAGE_SIZE);
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

	gDevFragments++;

#ifdef FLASH_INDEX
	/* If an index has been built, search in it */
	int lo, hi;
	IndexSearchSector(gIndex, buf, &lo, &hi);
	for(int i = lo; i < hi; i++) {
		if(!memcmp_fun(buf, gIndex[i].sector, size)) {
			gDevIndexHits++;
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
			UI_FileMappingProgressBar(pMap->miTotalLength, iFileSize);
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
		if(!gDevLowestFragment || pFragment < gDevLowestFragment)
			gDevLowestFragment = pFragment;

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
			if((iLength == FLASH_PAGE_SIZE)
				? CGD_sector_memcmp(pFileData, pFragment, iLength)
				: memcmp(pFileData, pFragment, iLength))
					break;
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
	if(gWADmethod == CGDOOM_WAD_BFILE)
		return 0;

	ASSERT(readpos >= 0);
	if (readpos + size > gpWADMap->miTotalLength)
		return -1;

	int iFragIndx = FindFragmentInMap(gpWADMap, readpos / FLASH_PAGE_SIZE);
	int iFragOffset = gpWADMap->mTable[iFragIndx].file_address * FLASH_PAGE_SIZE;
	int iSubOffset = readpos - iFragOffset;

	int iFragEnd;
	if (iFragIndx+1 < gpWADMap->miItemCount)
		iFragEnd = gpWADMap->mTable[iFragIndx+1].file_address * FLASH_PAGE_SIZE;
	else
		iFragEnd = gpWADMap->miTotalLength;

	*buf = FLASH_CACHED_START + (gpWADMap->mTable[iFragIndx].flash_address * FLASH_PAGE_SIZE) + iSubOffset;

	/* Return how many bytes can be read off the fragment (up to size). */
	int iAvailableLen = min(iFragEnd - readpos, size);
	ASSERT(iAvailableLen > 0);
	return iAvailableLen;
}

int Flash_ReadFile(void *buf, int size, int readpos)
{
	if(gWADmethod == CGDOOM_WAD_BFILE)
		return Bfile_ReadFile_OS(gWADfd, buf, size, readpos);

	const void *pSrc;
	int iRet = 0;
	while(size >0)
	{
		int i = FindInFlash(&pSrc,size, readpos);
		if(i<0) {
			I_ErrorI ("Flash_ReadFile", size, readpos, 0, i);
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

void abort(void){
	int x=0,y=160;
	PrintMini(&x,&y,"Abort called",0,0xFFFFFFFF,0,0,0xFFFF,0,1,0);
	int key;
	for(;;)
		GetKey(&key);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void){
	int time, ms_index=0, ms_mmap=0;

	InitFlashSimu("doom.wad"); //load wad file to flash simulation on simulator, do nothing on real HW
#ifdef CG_EMULATOR
	SaveVRAMBuffer = aSaveVRAMBuffer;
	SystemStack = aSystemStack;
#else 
	EnableColor(1);
	EnableStatusArea(3);

	WADFileInfo wads[16];
	int wad_count = FindWADs(wads, 16);

	int dev_info = 0;
	int choice = UI_Main(wads, wad_count, &dev_info, &gWADmethod);
	if(choice < 0)
		return 1;

	unsigned tmp=((unsigned)getSecondaryVramAddress()+3)&(~3);
	SaveVRAMBuffer = (unsigned char*)tmp;
	/* Graph 90+E: RAM starts at 0x0c000000 in physical memory */
	SystemStack = (void *)0xac0f0000;
#endif 
	VRAM = (unsigned short*)GetVRAMAddress();

	/* Setup access to WAD file */
	if(gWADmethod == CGDOOM_WAD_BFILE)
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
 #endif
		time = RTC_GetTicks();
		gpWADMap = (FileMapping *)(SaveVRAMBuffer + 2*65536);
		ASSERT(2*65536 + sizeof(FileMapping) < SAVE_VRAM_SIZE);
		int fd = Bfile_OpenFile_OS(wads[choice].path, 0, 0);
		int size = CreateFileMapping(fd, gpWADMap);
		Bfile_CloseFile_OS(fd);
		UI_FileMappingProgressBar(1, 1);
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
			Layout_Text(&l, "Fragments:", "%d", gDevFragments);
			Layout_Text(&l, "Index hits:", "%d (%d%%)", gDevIndexHits,
				(gDevIndexHits * 100 / gDevFragments));
			Layout_Text(&l, "Lowest fragment:", "%p",
				gDevLowestFragment);
			Layout_Spacing(&l, 12);
			Layout_Text(&l, "Index build time:", "%d ms", ms_index);
			Layout_Text(&l, "File mapping time:", "%d ms", ms_mmap);
			Bdisp_PutDisp_DD();
			int key;
			GetKey(&key);
		}
	}

	memset(VRAM,0,WIDTH*HEIGHT*2);
	D_DoomMain();

	if(gWADfd >= 0)
		Bfile_CloseFile_OS(gWADfd);

	return 1;
}

//todo: wrapper pro (patch_t*), + flash
