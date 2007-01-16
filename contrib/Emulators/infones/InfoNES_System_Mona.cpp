/*===================================================================*/
/*                                                                   */
/*  InfoNES_System_Mona.cpp : Mona specific File                     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

//#include <monapi.h>
//#include <monapi/messages.h>
#include <baygui.h>

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

/*-------------------------------------------------------------------*/
/*  Global Variables ( Mona specific )                               */
/*-------------------------------------------------------------------*/

static int isExit = 0, waitCount = 0;
static bool firstPaint = false;
static uint32_t my_tid = 0, keyevt_tid = 0, dwKeyPad1 = 0, dwKeyPad2 = 0;

/*-------------------------------------------------------------------*/
/*  Class prototypes ( Mona specific )                               */
/*-------------------------------------------------------------------*/

class XInfoNES : public Frame
{
public:
	XInfoNES()
	{
		setBounds(
			(800 - NES_DISP_WIDTH  - 12) / 2, 
			(600 - NES_DISP_HEIGHT - 28) / 2, 
			NES_DISP_WIDTH + 12, 
			NES_DISP_HEIGHT + 28
		);
		setTitle("InfoNES");
	}

	uint32_t* getImageData()
	{
		return this->_buffer->getSource();
	}
	
	void paint(Graphics* g)
	{
		firstPaint = true;
	}
	
	virtual void processEvent(Event* e)
	{
		if (e->getType() == KeyEvent::KEY_PRESSED) {
			KeyEvent* ke = (KeyEvent *)e;
			int keycode = ke->getKeycode();
			if (keycode == KeyEvent::VKEY_RIGHT) {
				dwKeyPad1 |= ( 1 << 7 );
			} else if (keycode == KeyEvent::VKEY_LEFT) {
				dwKeyPad1 |= ( 1 << 6 );
			} else if (keycode == KeyEvent::VKEY_DOWN) {
				dwKeyPad1 |= ( 1 << 5 );
			} else if (keycode == KeyEvent::VKEY_UP) {
				dwKeyPad1 |= ( 1 << 4 );
			} else if (keycode == 'x') { // START (x)
				dwKeyPad1 |= ( 1 << 3 );
			} else if (keycode == 'z') { // SELECT (z)
				dwKeyPad1 |= ( 1 << 2 );
			} else if (keycode == 'a') { // B (a)
				dwKeyPad1 |= ( 1 << 1 );
			} else if (keycode == 's') { // A (s)
				dwKeyPad1 |= ( 1 << 0 );
			} else if (keycode == 'k') { // SPEED DOWN (k)
				if (FrameSkip > 0) {
					FrameSkip--;
				} else {
					waitCount++;
				}
			} else if (keycode == 'l') { // SPEED UP (l)
				if (waitCount > 0) {
					waitCount--;
				} else {
					FrameSkip++;
				}
			}
		} else if (e->getType() == KeyEvent::KEY_RELEASED) {
			KeyEvent* ke = (KeyEvent *)e;
			int keycode = ke->getKeycode();
			if (keycode == KeyEvent::VKEY_RIGHT) {
				dwKeyPad1 &= ~( 1 << 7 );
			} else if (keycode == KeyEvent::VKEY_LEFT) {
				dwKeyPad1 &= ~( 1 << 6 );
			} else if (keycode == KeyEvent::VKEY_DOWN) {
				dwKeyPad1 &= ~( 1 << 5 );
			} else if (keycode == KeyEvent::VKEY_UP) {
				dwKeyPad1 &= ~( 1 << 4 );
			} else if (keycode == 'x') { // START (x)
				dwKeyPad1 &= ~( 1 << 3 );
			} else if (keycode == 'z') { // SELECT (z)
				dwKeyPad1 &= ~( 1 << 2 );
			} else if (keycode == 'a') { // B (a)
				dwKeyPad1 &= ~( 1 << 1 );
			} else if (keycode == 's') { // A (s)
				dwKeyPad1 &= ~( 1 << 0 );
			}
		}
	}
};

static XInfoNES* xinfones;

/*-------------------------------------------------------------------*/
/*  Function prototypes ( Mona specific )                            */
/*-------------------------------------------------------------------*/

static void EventLoop()
{
	MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);

	/* The main loop of InfoNES */ 
	InfoNES_Main();

	/* Stop thread */
	syscall_kill_thread(keyevt_tid);
}

/* Palette data */
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

/*===================================================================*/
/*                                                                   */
/*                main() : Application main                          */
/*                                                                   */
/*===================================================================*/

/* Application main */
int main(int argc, char* argv[])
{
	/* Command line */
	if (argc == 1) {
		printf("InfoNES for Mona v0.96J\n");
		printf("copyright (c) 2005, bayside.\n");
		printf("\n");
		printf("usage: infones.ex5 [*.nes]\n");
		printf("keyconfigration:\n");
		printf("    START  : 's'\n");
		printf("    SELECT : 'a'\n");
		printf("    ButtonA: 'x'\n");
		printf("    ButtonB: 'z'\n");
		printf("    SLOWER : 'k'\n");
		printf("    FASTER : 'l'\n");
		printf("    EXIT   : 'ESC'\n");
		return 0;
	}

	/* Open ROM file */
	if (InfoNES_Load(argv[1]) != 0) exit(1);

	/* Set frame skip */
	FrameSkip = 2;
	
	/* Create thread */
	my_tid = syscall_get_tid();
	uint32_t id = syscall_mthread_create((uint32_t)EventLoop);
// comment out by higepon
//	syscall_mthread_join(id);
	MessageInfo msg, src;
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	keyevt_tid = msg.from;

	/* Open Window */
	xinfones = new XInfoNES();
	xinfones->run();

	return 0;
}

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Menu() : Menu screen                     */
/*                                                                   */
/*===================================================================*/
int InfoNES_Menu()
{
	return isExit;
}

/*===================================================================*/
/*                                                                   */
/*               InfoNES_ReadRom() : Read ROM image file             */
/*                                                                   */
/*===================================================================*/
int InfoNES_ReadRom( const char *pszFileName )
{
	int i;
	BYTE *fp1;
	monapi_cmemoryinfo* mi = NULL;
	
	/* Read File */
	mi = monapi_file_read_all(pszFileName);
	if (mi == NULL || mi->Size == 0) {
		syscall_print("ROM file not found!");
		exit(1);
	}
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
			SRAM[0x1000 + i] = *fp1++;
	}

	/* Allocate Memory for ROM Image */
	ROM = (BYTE *)malloc( NesHeader.byRomSize * 0x4000 );
	memset( ROM, 0, NesHeader.byRomSize * 0x4000 );

	/* Read ROM Image */
	for(i = 0; i < NesHeader.byRomSize * 0x4000; i++)
		ROM[i] = *fp1++;

	if ( NesHeader.byVRomSize > 0 ) {
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

/*===================================================================*/
/*                                                                   */
/*           InfoNES_ReleaseRom() : Release a memory for ROM         */
/*                                                                   */
/*===================================================================*/
void InfoNES_ReleaseRom()
{
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

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemoryCopy() : memcpy                         */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
/*
 *  memcpy
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the copied block's destination
 *
 *    const void *src                  (Read)
 *      Points to the starting address of the block of memory to copy
 *
 *    int count                        (Read)
 *      Specifies the size, in uint8_ts, of the block of memory to copy
 *
 *  Return values
 *    Pointer of destination
 */
	memcpy( dest, src, count );
	return dest;
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemorySet() : memset                          */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemorySet( void *dest, int c, int count )
{
/*
 *  memset
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the block of memory to fill
 *
 *    int c                            (Read)
 *      Specifies the uint8_t value with which to fill the memory block
 *
 *    int count                        (Read)
 *      Specifies the size, in uint8_ts, of the block of memory to fill
 *
 *  Return values
 *    Pointer of destination
 */
	memset( dest, c, count );
	return dest;
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_LoadFrame() :                                        */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/
void InfoNES_LoadFrame()
{
/*
 *  Transfer the contents of work frame on the screen
 *
 */
	/* NULL check */
	if (!firstPaint) return;
	
	BYTE* vram = (BYTE *) xinfones->getImageData();
	
	/* Copy WorkFrame to VRAM */
	for (int y = 0; y < NES_DISP_HEIGHT; y++) {
		for (int x = 0; x < NES_DISP_WIDTH; x ++) {
			WORD wColor = WorkFrame[ ( y << 8 ) + x ];
			int index = (( y * NES_DISP_WIDTH ) + x) * 4;
			vram[ index + 2 ] = ((wColor & 0x7c00) >> 7);
			vram[ index + 1 ] = ((wColor & 0x03e0) >> 2);
			vram[ index + 0 ] = ((wColor & 0x001f) << 3);
		}
	}
	xinfones->update();
	
	/* Wait */
	if (waitCount > 0) sleep(waitCount * 17/* 1/60s */);
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_PadState() : Get a joypad state               */
/*                                                                   */
/*===================================================================*/
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
/*
 *  Get a joypad state
 *
 *  Parameters
 *    DWORD *pdwPad1                   (Write)
 *      Joypad 1 State
 *
 *    DWORD *pdwPad2                   (Write)
 *      Joypad 2 State
 *
 *    DWORD *pdwSystem                 (Write)
 *      Input for InfoNES
 *
 */
	/* Transfer joypad state */
	*pdwPad1 = dwKeyPad1;
	*pdwPad2 = dwKeyPad2;
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundInit() : Sound Emulation Initialize           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundInit( void )
{
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundOpen() : Sound Open                           */
/*                                                                   */
/*===================================================================*/
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate )
{
	return 0;
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundClose() : Sound Close                         */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundClose( void )
{
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_Wait() : Wait Emulation if required            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Wait()
{
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SoundOutput() : Sound Output 5 Waves           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5)
{
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_MessageBox() : Print System Message            */
/*                                                                   */
/*===================================================================*/
void InfoNES_MessageBox( char *pszMsg, ... )
{
	printf( "InfoNES:%s\n", pszMsg );
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_DebugPrint() : Print Debug Message             */
/*                                                                   */
/*===================================================================*/
void InfoNES_DebugPrint( char *pszMsg )
{
	printf( "InfoNES:%s\n", pszMsg );
}
