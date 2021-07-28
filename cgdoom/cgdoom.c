#include "platform.h"
#include "os.h"

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

static uint16_t const *gWADpath = u"\\\\fls0\\doom.wad";

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
//descriptor for 1 fragment
typedef struct
{
	unsigned short msOffset;//page index (0 ~ 64K)
	short msCount;//count of pages in this fragment
}FileMappingItem;


typedef struct
{
	FileMappingItem mTable[MAX_FRAGMENTS];//table of fragments
	int miItemCount;
	int miTotalLength;//length of the file
	int miCurrentLength;//currently returned length (by GetNextdata() )
	int miCurrentItem;//active fragment (to be returned by GetNextdata() )

}FileMapping;

//reset reading to start
void ResetData(FileMapping *pMap)
{
	pMap->miCurrentItem = 0;
	pMap->miCurrentLength = 0;
}


void I_Error (char *error, ...);

#ifdef CGDOOM_WAD_BFILE

/* File descriptor to WAD file, used in Flash_ReadFile calls from w_wad.c. */
static int gWADfd = -1;

int FindInFlash(void **buf, int size, int readpos)
{
	return 0;
}

int Flash_ReadFile(void *buf, int size, int readpos)
{
	return Bfile_ReadFile_OS(gWADfd, buf, size, readpos);
}

#else /* CGDOOM_WAD_MAPPING */

static FileMapping *gpWADMap = 0;
static SectorIndexInfo *gIndex = NULL;

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
		int diff = index[m].start_bytes - needle;

		if(diff < 0) lo = m + 1;
		else hi = m;
	}

	if(lo >= FLASH_INDEX_SIZE || index[lo].start_bytes != needle) return;
	*lo_ptr = hi = lo;

	/* Store last occurrence in *hi_ptr */
	do hi++;
	while(hi < FLASH_INDEX_SIZE && index[hi].start_bytes == needle);

	*hi_ptr = hi;
}

static int index_hits = 0;
static int sector_searches = 0;

/* Find a flash sector which contains the same data as buf. */
int FindSectorInFlash(const void *buf, int size)
{
	typeof(&memcmp) memcmp_fun = &memcmp;
	if(size == FLASH_PAGE_SIZE) memcmp_fun = &CGD_sector_memcmp;

	sector_searches++;

#ifdef FLASH_INDEX
	/* If an index has been built, search in it */
	int lo, hi;
	IndexSearchSector(gIndex, buf, &lo, &hi);
	for(int i = lo; i < hi; i++) {
		if(!memcmp_fun(buf, gIndex[i].sector, size)) {
			index_hits++;
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

	pMap->miItemCount = 0;
	pMap->miTotalLength = 0;

	const void *pFileData = ReadNextSector(&fc, &iLength);
	while(iLength > 0)
	{
		int iSectorID = FindSectorInFlash(pFileData, iLength);
		if(iSectorID == -1)
			return -2; // Page not found!

		pMap->miItemCount++;
		if(pMap->miItemCount >= MAX_FRAGMENTS)
			return -3; // File too fragmented!

		pMap->mTable[pMap->miItemCount-1].msOffset = iSectorID;
		pMap->mTable[pMap->miItemCount-1].msCount = 0;

		/* Look for consecutive sectors in the same fragment */
		const void *pFragment = FLASH_START + (iSectorID * FLASH_PAGE_SIZE);
		for(;;)
		{
			pMap->mTable[pMap->miItemCount-1].msCount++;
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

	if(pMap->miTotalLength >50000)
	{
		pMap->miTotalLength = 50000;//hack
	}
	return pMap->miTotalLength;
}

int FindInFlash(const void **buf, int size, int readpos)
{
	int iPageReq = readpos / FLASH_PAGE_SIZE;
	int iPageIndx = 0;
	int iCurrOffset = 0, iCurrLen;
	int iSubOffset;
	ASSERT(readpos >=0);
	//find item
	for(;;)
	{
		if(iPageIndx >= gpWADMap->miItemCount)
		{
			return -1;
		}
		if(iPageReq < gpWADMap->mTable[iPageIndx].msCount)
		{
			ASSERT(iCurrOffset <= readpos);
			break;
		}
		iPageReq -= gpWADMap->mTable[iPageIndx].msCount;
		iCurrOffset += ((int)gpWADMap->mTable[iPageIndx].msCount) * FLASH_PAGE_SIZE;
		iPageIndx++;
	}
	iSubOffset = readpos - iCurrOffset;
	iCurrLen = (gpWADMap->mTable[iPageIndx].msCount * FLASH_PAGE_SIZE) - iSubOffset;
	ASSERT(iCurrLen > 0);
	if(iCurrLen > size)
	{
		iCurrLen = size;
	}
	*buf = FLASH_CACHED_START + (gpWADMap->mTable[iPageIndx].msOffset * FLASH_PAGE_SIZE) + iSubOffset;
	return iCurrLen;
}

int Flash_ReadFile(void *buf, int size, int readpos)
{
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

#endif /* CGDOOM_WAD_* access method */

void abort(void){
	int x=0,y=160;
	PrintMini(&x,&y,"Abort called",0,0xFFFFFFFF,0,0,0xFFFF,0,1,0);
	int key;
	for(;;)
		GetKey(&key);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void){
	InitFlashSimu(gWADpath); //load wad file to flash simulation on simulator, do nothing on real HW
#ifdef CG_EMULATOR
	SaveVRAMBuffer = aSaveVRAMBuffer;
	SystemStack = aSystemStack;
#else 
	unsigned tmp=((unsigned)getSecondaryVramAddress()+3)&(~3);
	SaveVRAMBuffer = (unsigned char*)tmp;
	/* Graph 90+E: RAM starts at 0x0c000000 in physical memory */
	SystemStack = (void *)0xac0f0000;
#endif 
	VRAM = (unsigned short*)GetVRAMAddress();
	EnableColor(1);
	memset(VRAM,0,WIDTH*HEIGHT*2);

	/* Setup access to WAD file */
#ifdef CGDOOM_WAD_BFILE
	gWADfd = Bfile_OpenFile_OS(gWADpath, 0, 0);
#else
	int time_start = RTC_GetTicks();

 #ifdef FLASH_INDEX
	/* Index most likely flash sectors into a sorted array, so that sectors
	   can be hit quickly. The index contains every sector on a 4-kiB
	   boundary (where fragments are most likely to start) between
	   FLASH_FS_HINT and FLASH_END. */
	gIndex = (void *)SystemStack;
	for(int i = 0; i < FLASH_INDEX_SIZE; i++) {
		SectorIndexInfo *info = &gIndex[i];
		info->sector = FLASH_FS_HINT + (i * 4096);
		info->start_bytes = *(const uint32_t *)info->sector;
	}
	qsort(gIndex, FLASH_INDEX_SIZE, sizeof *gIndex, IndexCompareSectors);
 #endif

	gpWADMap = (FileMapping *)(SaveVRAMBuffer + 2*65536);
	ASSERT(2*65536 + sizeof(FileMapping) < SAVE_VRAM_SIZE);
	int fd = Bfile_OpenFile_OS(gWADpath,0,0);
	int size = CreateFileMapping(fd, gpWADMap);
	Bfile_CloseFile_OS(fd);
	int time_end = RTC_GetTicks();

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
	else {
		int key;
		char line[22];
		int time_ms = (time_end - time_start) * 8;
		Bdisp_AllClr_VRAM();
		locate_OS(1, 1);
		CGDAppendNum0_999("mmap (ms): ", time_ms, 1, line);
		PrintLine(line, 21);
		locate_OS(1, 2);
		CGDAppendNum0_999("Searches: ", sector_searches, 1, line);
		PrintLine(line, 21);
		locate_OS(1, 3);
		CGDAppendNum0_999("Index hits: ", index_hits, 1, line);
		PrintLine(line, 21);
		Bdisp_PutDisp_DD();
		GetKey(&key);
	}
#endif

	D_DoomMain();
	return 1;
}

//todo: wrapper pro (patch_t*), + flash
