#ifndef _MONA_GLOBAL_
#define _MONA_GLOBAL_

#ifdef GLOBAL_VALUE_DEFINED
#undef  GLOBAL
#define GLOBAL /* */
#undef  GLOBAL_VAL
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif

GLOBAL VirtualConsole* g_console GLOBAL_VAL(0);      /*< pointer to console    */
GLOBAL VirtualConsole* g_log     GLOBAL_VAL(0);

GLOBAL int  g_info_level GLOBAL_VAL(INFO_DEV_ERROR);

/* expr:sysresource */
GLOBAL BitMap *g_irqMap;

GLOBAL dword g_total_system_memory;

GLOBAL VesaInfoDetail g_vesaDetail;

GLOBAL dword gt[128];

GLOBAL PsInfo g_ps;

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define logprintf g_log->printf
#else
#define logprintf //
#endif

#endif
