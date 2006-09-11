//$Id$
#include "MonADEC.h"
using namespace MonAPI;
using namespace mones;

MonADEC::~MonADEC()
{
    
}

MonADEC::MonADEC()
{
    
}

//Initalization Procedure is described in H.R.M 4.3.4
int MonADEC::init()
{
    reset();
//1. Wait 50 PCI clock cycles.
    sleep(100);
//2. Update configuration registers (Section 3.1): 
    outp32(iobase+CSR_0,0x01A08000);
//a. read id and rev.  ==> done in Dispatch?
//b. Write the configuration interrupt register(use default?)

//c. Write the configuration base address registers.
//d. Write the configuration command register.
//e. Write the configuration latency counter to match the system latency guidelines.
//3. Write CSR0 to set global host bus operating parameters (Section 3.2.2.1).
//4. Write CSR7 to mask unnecessary (depending on the particular application) interrupt causes.
//5. The driver must create the transmit and receive descriptor lists. Then, it writes to both CSR3
//   and CSR4, providing the 21143 with the starting address of each list (Section 3.2.2.7). The
//   first descriptor on the transmit list may contain a setup frame (Section 4.2.3).
//6. When in either 10BASE-T or AUI mode, change the default settings of the jabber timers and
//   also the initial SIA settings by writing to CSR13 (Section 3.2.2.16), CSR14 (Section 3.2.2.17),
//   and CSR15 (Section 3.2.2.18).
//7. Write CSR6 (Section 3.2.2.9) to set global serial parameters and to start both the receive and
//   transmit processes. The receive and transmit processes enter the running state and attempt to
//   acquire descriptors from the respective descriptor lists. Then the receive and transmit
//   processes begin processing incoming and outgoing frames. The receive and transmit.
    printf("Hello Virtual PC\n");
    return 0;
}

int MonADEC::interrupt()
{
    return 0;
}

void MonADEC::SendFrm(Ether* frame)
{

}
