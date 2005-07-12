/*===================================================================*/
/*                                                                   */
/*  InfoNES_System.h : The function which depends on a system        */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include <monapi.h>
#include <monapi/messages.h>

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

/*-------------------------------------------------------------------*/
/*  Palette data                                                     */
/*-------------------------------------------------------------------*/
//extern WORD NesPalette[];
WORD NesPalette[ 64 ] =
{
  0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
  0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
  0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
  0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
  0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
  0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

/*-------------------------------------------------------------------*/
/*  Grobal variable                                                  */
/*-------------------------------------------------------------------*/

static int offx, offy;
static MonAPI::Screen screen;

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

#if 0
static int printf(char *format, ...)
/* output cosole */
{
	int i, I;
	va_list ap;

	va_start(ap, format);
	I = vsprintf(strbuf, format, ap);
	va_end(ap);
	for(i = 0; i < I; i++){
		putch(strbuf[i]);
	}
	return I;
}
#endif

/*===================================================================*/
/*                                                                   */
/*                main() : Application main                          */
/*                                                                   */
/*===================================================================*/

int MonaMain( List<char*>* pekoe ) {
	/* Command line */
	if (pekoe->size() == 0) {
		printf("InfoNES for Mona\n");
		printf("copyright (c) 2005, bayside.\n");
		printf("\n");
		printf("usage: infones.ex2 [*.nes]\n");
		printf("keyconfigration:\n");
		printf("    START  : 's'\n");
		printf("    SELECT : 'a'\n");
		printf("    buttonA: 'x'\n");
		printf("    buttonB: 'z'\n");
		printf("    SLOWER : 'k'\n");
		printf("    FASTER : 'l'\n");
		printf("    EXIT   : 'ESC'\n");
		return 0;
	}

	/* Open ROM file */
	if (InfoNES_Load(pekoe->get(0)) != 0) return 0;

	/* Set frame skip */
	FrameSkip = 5;

	/* The main loop of InfoNES */ 
	InfoNES_Main();

	return 0;
}

/* Menu screen */
int InfoNES_Menu(){
	return 0;
}

/* Read ROM image file */
int InfoNES_ReadRom( const char *pszFileName ){
	int i;
	BYTE *fp1;
	monapi_cmemoryinfo* mi = NULL;
	
	/* Read File */
	mi = monapi_call_file_read_data(pszFileName, false);
	if (mi == NULL || mi->Size == 0) return 1;
	fp1 = (BYTE *)malloc(mi->Size);
	memcpy(fp1, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);

	/* Read header */
	for (i = 0; i < 4; i++) NesHeader.byID[i] = *fp1++;
	NesHeader.byRomSize  = *fp1++;
	NesHeader.byVRomSize = *fp1++;
	NesHeader.byInfo1    = *fp1++;
	NesHeader.byInfo2    = *fp1++;
	for (i = 0; i < 8; i++) NesHeader.byReserve[i] = *fp1++;

	/* Clear SRAM */
	for (i = 0; i < SRAM_SIZE; i++) SRAM[i] = 0;

	/* If trainer presents Read Triner at 0x7000-0x71ff */
	if ( NesHeader.byInfo1 & 4 ) {
		for (i = 0; i < 512; i++)
			//SRAM[0x1000 + i] = *fp1++;
			*fp1++;
	}

	/* Allocate Memory for ROM Image */
	ROM = (BYTE *)malloc( NesHeader.byRomSize * 0x4000 );
	memset( ROM, 0, NesHeader.byRomSize * 0x4000 );

	/* Read ROM Image */
	for(i = 0; i < NesHeader.byRomSize * 0x4000; i++)
		ROM[i] = *fp1++;

	if ( NesHeader.byVRomSize > 0 ){
		/* Allocate Memory for VROM Image */
		VROM = (BYTE *)malloc( NesHeader.byVRomSize * 0x2000 );
		memset( VROM, 0, NesHeader.byVRomSize * 0x2000 );

		/* Read VROM Image */
		for(i = 0; i < NesHeader.byVRomSize * 0x2000; i++)
			VROM[i] = *fp1++;
	}
	
	/* Successful */
	return 0;
}

/* Release a memory for ROM */
void InfoNES_ReleaseRom(){
/*
 *  Release a memory for ROM
 *
 */

	if ( ROM ) {
		free( ROM );
		ROM = NULL;
	}

	if ( VROM ) {
		free( VROM );
		VROM = NULL;
	}
}

/* Transfer the contents of work frame on the screen */
void InfoNES_LoadFrame(){
	BYTE* vram = screen.getVRAM();
	int bpp = screen.getBpp();
	int sw = screen.getWidth();
	int sh = screen.getHeight();

	/* 16bpp only */
	if (bpp != 16) return;
	
	/* Set screen mode */
	offx = (sw - NES_DISP_WIDTH ) / 2;
	offy = (sh - NES_DISP_HEIGHT) / 2;
	
	/* Copy WorkFrame to VRAM */
	for (int y = 0; y < NES_DISP_HEIGHT; y++) {
		for (int x = 0; x < NES_DISP_WIDTH; x++) {
			BYTE* pVram = &vram[((x + offx) + (y + offy) * sw) * (bpp >> 3)];
			*(unsigned short*)pVram = WorkFrame[y * NES_DISP_WIDTH + x];
		}
	}
}

/* Get a joypad state */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem ){
#if 0
	KEY_PACKET kpacket;
	//*pdwPad1 = 0;
	//*pdwPad2 = 0;
	
	if (sys_fs_read(devid_rpcss, port_keybd, 0, &kpacket, 1) > 0) {
		//printf("key: %d %d\n", kpacket.keycode, kpacket.shift_state);
		if (kpacket.keycode == 0) { // ESC
			exit(0);
		}
		// key typed
		//if (kpacket.keycode <= 0x4B) {
			if (kpacket.keycode == 0x43 || kpacket.keycode == 0x3A) { // UP
				*pdwPad1 |= 0x10;
			} else if (kpacket.keycode == 0x4B || kpacket.keycode == 0x3D) { // DOWN
				*pdwPad1 |= 0x20;
			} else if (kpacket.keycode == 0x46 || kpacket.keycode == 0x3B) { // LEFT
				*pdwPad1 |= 0x40;
			} else if (kpacket.keycode == 0x48 || kpacket.keycode == 0x3C) { // RIGHT
				*pdwPad1 |= 0x80;
			} else if (kpacket.keycode == 0x29) { // B (a)
				*pdwPad1 |= 0x2;
			} else if (kpacket.keycode == 0x2A) { // A (s)
				*pdwPad1 |= 0x1;
			} else if (kpacket.keycode == 0x1D) { // SELECT (z)
				*pdwPad1 |= 0x4;
			} else if (kpacket.keycode == 0x1E) { // START (x)
				*pdwPad1 |= 0x8;
			} else if (kpacket.keycode == 0x24) { // SPEED DOWN (k)
				if (FrameSkip > 0) {
					FrameSkip--;
				}
			} else if (kpacket.keycode == 0x25) { // SPEED UP (l)
				FrameSkip++;
			}
		// key released
		//} else if (kpacket.keycode >= 0x80000000) {
			else if (kpacket.keycode == 0x80000043 || kpacket.keycode == 0x8000003A) { // UP
				*pdwPad1 ^= 0x10;
			} else if (kpacket.keycode == 0x8000004B || kpacket.keycode == 0x8000003D) { // DOWN
				*pdwPad1 ^= 0x20;
			} else if (kpacket.keycode == 0x80000046 || kpacket.keycode == 0x8000003B) { // LEFT
				*pdwPad1 ^= 0x40;
			} else if (kpacket.keycode == 0x80000048 || kpacket.keycode == 0x8000003C) { // RIGHT
				*pdwPad1 ^= 0x80;
			} else if (kpacket.keycode == 0x80000029) { // B (a)
				*pdwPad1 ^= 0x2;
			} else if (kpacket.keycode == 0x8000002A) { // A (s)
				*pdwPad1 ^= 0x1;
			} else if (kpacket.keycode == 0x8000001D) { // SELECT (z)
				*pdwPad1 ^= 0x4;
			} else if (kpacket.keycode == 0x8000001E) { // START (x)
				*pdwPad1 ^= 0x8;
			}
		//}
	}
#endif
}

/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count ){
	return memcpy(dest, src, count);
}

/* memset */
void *InfoNES_MemorySet( void *dest, int c, int count ){
	return memset(dest, c, count);
}

/* Print debug message */
void InfoNES_DebugPrint( char *pszMsg ){
	printf( "InfoNES:%s\n", pszMsg );
}

/* Wait */
void InfoNES_Wait(){
}

/* Sound Initialize */
void InfoNES_SoundInit( void ){
}

/* Sound Open */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ){
	return 0;
}

/* Sound Close */
void InfoNES_SoundClose( void ){
}

/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM */
void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5){
}

/* Print system message */
void InfoNES_MessageBox( char *pszMsg, ... ){
	printf( "InfoNES:%s\n", pszMsg );
}
