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
// 4 kiB are used too, a file might not start on the first sector of a cluster,
// and some sectors might also be dysfunctional.
//
// See <platform.h> for Flash traversal parameters.
///////////////////////////////////////////////////////////////////////////////

static uint16_t const *cgdoom_wad_path = u"\\\\fls0\\doom.wad";

#ifdef CGDOOM_WAD_BFILE
/* File descriptor to WAD file, used in Flash_ReadFile calls from w_wad.c. */
static int cgdoom_wad_fd = -1;
#endif

//The whole sound doesn't fir onto the RAM.
//Reading per partes is not possible as this is synchronnous player (there would be silences when reading).
//So I read each page (4KB)of the wav file and try to find it in the flash. 
//Simply finding start of the file is not enough because of fragmentation.

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

int FindInFlash(void **buf, int size, int readpos)
{
	return 0;
}

int Flash_ReadFile(void *buf, int size, int readpos)
{
	return Bfile_ReadFile_OS(cgdoom_wad_fd, buf, size, readpos);
}

#else /* CGDOOM_WAD_MAPPING, CGDOOM_WAD_MAPPING_DMA */

static FileMapping *gpWADMap = 0;

int CreateFileMapping(const unsigned short *pFileName,FileMapping *pMap)
{
	int iResult = 0;
	char cBuffer[FLASH_PAGE_SIZE];
	int hFile = Bfile_OpenFile_OS(pFileName,0,0);
	int iLength;
	char *pFlashFS = (char *)FLASH_START;

	pMap->miItemCount = 0;
	pMap->miTotalLength = 0;
	iLength = Bfile_ReadFile_OS(hFile,cBuffer,FLASH_PAGE_SIZE,-1);
	while(iLength > 0)
	{
		//do not optimize (= do not move these 2 variables before loop)!
		// fx-cg allocates pages for file in <random> order so page from the end of the file
		//can have lower index than page from the beginning
		const char *pTgt = pFlashFS;
		int iPageIndx = 0;

		for(;iPageIndx < FLASH_PAGE_COUNT;iPageIndx++)
		{
			if(!memcmp(pTgt,cBuffer,iLength))
			{
				break;
			}
			pTgt += FLASH_PAGE_SIZE;
		}
		if(iPageIndx == FLASH_PAGE_COUNT)
		{
			//page not found !
			iResult = -2;
			goto lbExit;
		}
		pMap->miItemCount ++;
		if(pMap->miItemCount >= MAX_FRAGMENTS)
		{
			//file too fragmented !
			iResult = -3;
			goto lbExit;
		}
		pMap->mTable[pMap->miItemCount-1].msOffset = (unsigned short)iPageIndx;
		pMap->mTable[pMap->miItemCount-1].msCount = 0;
		//assume fragment has more pages
		for(;;)
		{
			pMap->mTable[pMap->miItemCount-1].msCount++;
			pMap->miTotalLength += iLength;
			iPageIndx++;
			pTgt += FLASH_PAGE_SIZE;

			if(iLength < FLASH_PAGE_SIZE)
			{
				//this was the last page
				iResult = pMap->miTotalLength;
				goto lbExit;
			}
			iLength = Bfile_ReadFile_OS(hFile,cBuffer,FLASH_PAGE_SIZE,-1);
			if(iLength <= 0)
			{
				break;
			}
			if(memcmp(pTgt,cBuffer,iLength))
			{
				break;
			}
		}
	}
	if(iLength < 0)
	{
		iResult = -1;
	}
	else
	{
		if(pMap->miTotalLength >50000)
		{
			pMap->miTotalLength = 50000;//hack
		}

		iResult = pMap->miTotalLength;
	}

lbExit:
	Bfile_CloseFile_OS(hFile);
	return iResult;
}

int FindInFlash(void **buf, int size, int readpos)
{
	int iPageReq = readpos >>FLASH_PAGE_SIZE_LOG2;
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
		iCurrOffset += ((int)gpWADMap->mTable[iPageIndx].msCount) << FLASH_PAGE_SIZE_LOG2;
		iPageIndx++;
	}
	iSubOffset = readpos - iCurrOffset;
	iCurrLen = (gpWADMap->mTable[iPageIndx].msCount * FLASH_PAGE_SIZE) - iSubOffset;
	ASSERT(iCurrLen > 0);
	if(iCurrLen > size)
	{
		iCurrLen = size;
	}
	*buf = ((char *)FLASH_START)+(gpWADMap->mTable[iPageIndx].msOffset << FLASH_PAGE_SIZE_LOG2)+iSubOffset;
	return iCurrLen;
}

int Flash_ReadFile(void *buf, int size, int readpos)
{
	void *pSrc;
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
	InitFlashSimu(cgdoom_wad_path); //load wad file to flash simulation on simulator, do nothing on real HW
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
	cgdoom_wad_fd = Bfile_OpenFile_OS(cgdoom_wad_path, 0, 0);
#else
	gpWADMap = (FileMapping *)(SaveVRAMBuffer + 2*65536);
	ASSERT(2*65536 + sizeof(FileMapping) < SAVE_VRAM_SIZE);
	int size = CreateFileMapping(cgdoom_wad_path,gpWADMap);

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
	else if(size < 0) {
		I_ErrorI ("CreateFileMapping", size, 0, 0, 0);
		return 1;
	}
#endif

	D_DoomMain();
	return 1;
}

//todo: wrapper pro (patch_t*), + flash
