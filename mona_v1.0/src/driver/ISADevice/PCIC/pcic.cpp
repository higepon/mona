/* PCIC */
#include "pcic.h"
#include <io.h>

#define MEMWINDOW1 0xdc000
#define USEIRQ 11

#define SELECT 0x3e0
#define DATA (SELECT + 1)
ISADriver* isa_;

byte Read(byte device,byte offset){
  byte c;
  c = offset + ( device << 6);
  isa_->console->printf("PCIC:Read :off = %x device = %d  :: ",offset,device);
  outp8(SELECT,c);
  c = inp8(DATA);
  isa_->console->printf("%x\n",c);
  return c;
}

void Write(byte device,byte offset,byte data){
  byte c;
  c = offset + ( device << 6);
  isa_->console->printf("PCIC:Write:off = %x device = %d  :: ",offset,device);
  outp8(SELECT,c);
  outp8(DATA,data);
  isa_->console->printf("%x\n",data);
  return;
}

void pcicirqhandler(void){/* now device:0 slot:0 only handle */
  byte c;
  byte base;
  c = Read(0,0x04);
  isa_->console->printf("PCIC:IRQ:Status %x\n",c);
  if(c & 0x08){ /* card detect change? */
    if((Read(0,0x01) & 0x04)){ /* card inserted */
      byte *cis;
      word offset;
      cis = (byte *)MEMWINDOW1;
      /* Map Reg.Window to MEMWINDOW1 */
      base = 0x10;
      Write(0,base,0xdc);   /* base */
      Write(0,base+1,0);    /* 8-Bit access */
      Write(0,base+2,0xdc); /* limit */
      Write(0,base+3,0);
      Write(0,base+4,0x24); /* offset */
      Write(0,base+5,0xff); /* reg Active & WriteProtect*/
      Write(0,0x06,1);      /* MEMWinwow 1 Enable */
      offset = 0;
      isa_->console->printf("PCIC:Inserted.\n");
        
        int i,j;
        for(i=0;i!=512;i+=16){
          isa_->console->printf("\n");
          isa_->console->printf("%x : ",i);
          for(j=0;j!=16;j++){
            c = cis[i + j] >> 4;
            if(c > 9){
              c += ('A' - 10);
            }else{
              c += '0';
            }
            isa_->console->printf("%c",c);
            c = cis[i + j] & 0x0f;
            if(c > 9){
              c += ('A' - 10);
            }else{
              c += '0';
            }
            isa_->console->printf("%c ",c);
            
          }
        }

      for(;;){/* process CIS */
        word o;
        c = cis[offset];
        offset++;
        o = cis[offset];
        offset++;
        o = o + offset;
        switch(c){
          case 0x15:
            isa_->console->printf("PCIC:Card information:\n");
            isa_->console->printf(" Version:%d.%d\n",cis[offset],cis[offset++]);
            while(cis[offset] != 0xff){
              isa_->console->printf(" Str:");
              while(cis[offset]){
                isa_->console->printf("%c",cis[offset]);
                offset++;
              }
              isa_->console->printf("\n");
              offset++;
              if(offset > o){
                isa_->console->printf("PCIC:Structure Error!!\n");
                break;
              }
            }
            break;
          case 0xff:
            isa_->console->printf("PCIC:CIS End\n");
            break;
          default:
            isa_->console->printf("PCIC:Unkown CIS(%x)\n",c);
        }
        if(c == 0xff) break;
        offset = o;
      }
      
    }else{/* card ejected */
      isa_->console->printf("PCIC:Ejected.\n");
    }
  }else{
    /* do CardIRQ */
  }
}

PCIC::PCIC(ISADriver *isa){
  byte c;
  isa_ = isa;
  isa_->AcquireIO(0x3e0,2);
  /* isa_->AcquireMemory(MEMWINDOW1,0x1000); */
  c = Read(0,0);
  isa_->console->printf("PCIC:ID = %x\n",c);
  c = 0;
  c |= 0x80; /* OutPut Enable */
  c |= 0x20; /* Auto Power Control */
  Write(0,2,c); /* power setting */
  

  //c = 0;
  //c |= USEIRQ;
  //c |= 0x20;
  //Write(0,3,c); /* int. setting */
  Write(0,5,(USEIRQ<<4) + 0x08);
  c = Read(0,1);
  isa_->console->printf("PCIC:STATUS = %x\n",c);
  
  if(!isa_->AcquireIRQ(USEIRQ,pcicirqhandler)){
    isa_->console->printf("PCIC:can't acquire IRQ(%d)..\n",USEIRQ);
    return;
  }
  isa_->EnableIRQ(USEIRQ);
  
  c = 0x22;
  Write(0,0x16,c); /* rise IRQ */
  return;
  
}

PCIC::~PCIC(){}
