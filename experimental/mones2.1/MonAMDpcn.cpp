#include "MonAMDpcn.h"
using namespace MonAPI;
using namespace mones;

////////
TX::TX() 
{ 
	dsc=NULL;
	buf=monapi_allocate_dma_memory();
	for(int i=1;i<4;i++){
		byte* tmp=monapi_allocate_dma_memory();
		if( tmp != buf+0x1000*i){
			printf("TX:buf is not continuous.%d\n",i);
			buf=NULL;
		}
	}
}

TX::~TX()
{
	for(int i=0;i<4;i++)
		monapi_deallocate_dma_memory(dsc+0x1000*i);
}

int TX::initTX()
{
	if( buf == 0 )
		return -1;
	printf("TX%x\n",buf);
	dsc= (TXDSC*)buf;
	buf += (RINGLEN*sizeof(TXDSC));
	for(int i=0;i<RINGLEN;i++){
		(dsc+i)->status=0;
		(dsc+i)->control=0;
		(dsc+i)->bcnt=(dword)(-PKTSIZE);
		(dsc+i)->rbaddr=(dword)(buf+i*PKTSIZE);
	}
	return 0;
}

void TX::ihandler()
{
	printf("TX-INT\n");
}

////////
RX::RX() 
{  
	dsc=NULL;
	buf=monapi_allocate_dma_memory();
	for( int i=1;i<4;i++){
		byte* tmp=monapi_allocate_dma_memory();
		if( tmp != buf+0x1000*i){
			printf("RX:buf is not continuous.%d\n",i);
			buf=NULL;
		}
	}
}	

RX::~RX()
{
	for(int i=0;i<4;i++)
		monapi_deallocate_dma_memory(dsc+0x1000*i);
	printf("free\n");
}

int RX::initRX() 
{
	if( buf== 0 )
		return -1;
	printf("RX%x %x\n",buf,sizeof(RXDSC));
	dsc = (RXDSC*)buf;
	buf += (RINGLEN*sizeof(RXDSC)*2);
	for(int i=0;i<RINGLEN*2;i++){
	    (dsc+i)->bcnt=(dword)(-PKTSIZE)&0xF000;
		(dsc+i)->status=RMD1_OWN|RMD1_STP|RMD1_ENP;
		(dsc+i)->rbaddr=(dword)(buf+i*PKTSIZE);
	}
	index=0;
	return 0;
}

void RX::ihandler()
{
	printf("\n RX%x %x\n",index,(dsc+index)->status);
	for(int i=0;i<=RINGLEN;i++)
	        printf("%x %x ",(dsc+i)->status,(dsc+i)->mcnt);
	while( ((dsc+index)->status & RMD1_OWN) == 0 ){
	
		printf("==>%x %x\n",(dsc+index)->rbaddr,dsc);
		(dsc+index)->mcnt=0;
        (dsc+index)->bcnt = (dword)(-PKTSIZE)&0xF000;
		(dsc+index)->status = RMD1_OWN|RMD1_STP|RMD1_ENP;
	}
    index = (index+1) & (RINGLEN-1);
    //printf("\nRX%x %x\n",index,(dsc+index)->status);
}
///////////////////////////////////////////////////////

int MonAMDpcn::init()
{
	stop();
	reset();

	for(int i=0;i<5;i++){
		mac_addr[i]=inp8(iobase+i);
		printf("%x:",mac_addr[i]);
	}
	mac_addr[5]=inp8(iobase+5);
	printf("%x\n",mac_addr[5]);

	w_bcr(BCR_MISC,BCR_AUTOSEL);	    //SET BCR_EDGETRG for Edge Sense.
	w_bcr(BCR_SSTYLE,BCR_PCI_II|BCR_SSIZE);

	if( RX::initRX() != 0 ){
		printf("RX::SetupFailed.");
		return -1;
	}
	if( TX::initTX() != 0 ){ 
		printf("TX::SetupFailed.");
		return -1;
	}

	printf("DMA area was allocated properly.\n");
	//Direct Initialize. SEE SPEC SHEET APENDIX C.
	w_csr(CSR_CSR,CSR_STOP );          //CSR 0
	w_csr(CSR_IADR,NO_INIT_BLOCK);              //CSR 2
	//w_csr(CSR_FEATURE,FEAT_PADTX|FEAT_TXMSK);   //CSR 4  0x915	
	w_csr(CSR_MAR0,0x0000);                     //CSR 8
	w_csr(CSR_MAR1,0x0000);                     //CSR 9
	w_csr(CSR_MAR2,0x0000);                     //CSR10
	w_csr(CSR_MAR3,0x0000);                     //CSR11
	w_csr(CSR_PADR0,*(word*)mac_addr);          //CSR12
	w_csr(CSR_PADR1,*(word*)(mac_addr+2));      //CSR13 
	w_csr(CSR_PADR2,*(word*)(mac_addr+4));      //CSR14
	w_csr(CSR_MODE,MODE_PSEL);
	//w_csr(CSR_MODE,MODE_RXD|MODE_TXD);          //CSR15
	w_csr(CSR_RXADDR0,((dword)RX::dsc)&0xFFFF);        //CSR 24
	w_csr(CSR_RXADDR1,(((dword)RX::dsc)>>16) &0xFFFF); //CSR 25
	w_csr(CSR_TXADDR0,((dword)TX::dsc)&0xFFFF);        //CSR 30
	w_csr(CSR_TXADDR1,(((dword)TX::dsc)>>16) & 0xFFFF);//CSR 31
	//!!!PROBLEM!!!!
	//Buffer lengthes are discarded.
	//You can see wrote data at RX::RINGLENth bufffer
	//and 0xFFFF in CSR_TXRINGLEN
	w_csr(CSR_POLLINT,0);   //CSR 47
	w_csr(CSR_RXRINGLEN,(word)(-RX::RINGLEN));         //CSR 76
	w_csr(CSR_TXRINGLEN,(word)(-TX::RINGLEN));         //CSR 78	
	w_csr(CSR_IADR,NO_INIT_BLOCK);              //CSR 2
	//printf("chip version=%x\n",(r_csr(88)>>12)|(r_csr(89)<<4));
    ////////////////////////////////////////////////////////////
	w_csr(CSR_MODE,r_csr(CSR_MODE)|MODE_DNY_BCST);       //CSR15	
	printf("[%x]\n",r_csr(CSR_CSR));
	printf("[%x]\n",r_csr(CSR_IADR));
	printf("[%x]\n",r_csr(CSR_FEATURE));
	printf("[%x]\n",r_csr(CSR_MAR0));
	printf("[%x]\n",r_csr(CSR_MAR1));
	printf("[%x]\n",r_csr(CSR_MAR2));
	printf("[%x]\n",r_csr(CSR_MAR3));
	printf("[%x]\n",r_csr(CSR_PADR0));
	printf("[%x]\n",r_csr(CSR_PADR1));
	printf("[%x]\n",r_csr(CSR_PADR2));
	printf("[%x]\n",r_csr(CSR_MODE));
	printf("[%x]\n",r_csr(CSR_RXADDR0));
	printf("[%x]\n",r_csr(CSR_RXADDR1));
	printf("[%x]\n",r_csr(CSR_TXADDR0));
	printf("[%x]\n",r_csr(CSR_TXADDR1));
	printf("[%x]\n",r_csr(CSR_RXRINGLEN));
	printf("[%x]\n",r_csr(CSR_TXRINGLEN));
	w_csr(CSR_CSR,CSR_INTEN|CSR_START);                //CSR 0
	return 0;	
}

void MonAMDpcn::interrupt()
{
	//printf("Interrupted\n");
	word val;
	if( ( val = r_csr(CSR_CSR)) & CSR_INTR ){
		printf("<%x>\n",val);
		if( val & CSR_RINT ){
			RX::ihandler();
		}else if (val & CSR_TINT){
			TX::ihandler();
		}
		w_csr(CSR_CSR, val & 0xFFF0);
	}
	//Interrupt was masked by OS handler.
	enableNetwork(); //Now be enabled here. 
	//It should be enabled by messaging mechanism. 
}
 
void MonAMDpcn::outputFrame(byte* packet, byte* macAddress, dword size, word protocolId)
{

}

void MonAMDpcn::getFrameBuffer(byte* buffer, dword size)
{

}

/////////////
