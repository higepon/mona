#ifndef _MONA_IHANDLER_
#define _MONA_IHANDLER_

extern "C" void arch_fdchandler(void);
extern "C" void arch_timerhandler(void);

extern "C" void MFDCHandler(void);
extern "C" void timerHandler(void);


#endif
