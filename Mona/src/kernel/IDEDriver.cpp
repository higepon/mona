
#include "global.h"
#include "io.h"

#define	PIDE_DEVCTRL	0x3f6	// デバイスコントロール


#define CTRL_SRST   0x04
#define CTRL_IDIS   0x02
void hddTest()
{

    /* device control. reset and disable int */
    outp8(PIDE_DEVCTRL,CTRL_SRST | CTRL_IDIS);  








}

