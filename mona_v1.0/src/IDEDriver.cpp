/* IDE */

#include<IDEDriver.h>
#include<kernel.h>
#include<operator.h>
#include<io.h>
#include<global.h>

/* IDE commands*/
#define IDE_CMD_READ 0x20
#define IDE_CMD_DIAG 0x90
#define IDE_CMD_ID 0xec


VirtualConsole* IDEDriver::console_;
unsigned int IDEDriver::driveaddr_;
unsigned int IDEDriver::control_;
unsigned int IDEDriver::data_;
unsigned int IDEDriver::error_;
unsigned int IDEDriver::status_;
unsigned int IDEDriver::sector_;
unsigned int IDEDriver::count_;
unsigned int IDEDriver::cylinderL_;
unsigned int IDEDriver::cylinderH_;
unsigned int IDEDriver::head_;
unsigned int IDEDriver::status2_;
bool IDEDriver::HasMaster;
bool IDEDriver::HasSlave;


IDEDevice* IDEDriver::Master;
IDEDevice* IDEDriver::Slave;

IDEDriver::IDEDriver(VirtualConsole* console,unsigned int port) {

    /* set console */
    console_ = console;
    
    /* set ports */

    data_ = port;
    error_ = port+1;
    count_ = port+2;
    sector_ = port+3;
    cylinderL_ = port+4;
    cylinderH_ = port+5;
    head_ = port+6;
    status_ = port+7;
    driveaddr_ = port+0x207;
    status2_ = port+0x206;
    
    if(inportb(status_) == 0xff){
      console_->printf("ide:constructor:can't find IDE Drive in Port:%x\n",port);
      HasMaster = false;
      return;
    }
    
    if(!initilize()){
      HasMaster = false;
    }
}

#define TIMEOUT 36
#define TIMEOUT_CMD 36
bool IDEDriver::waithdc(unsigned long timeout){
  dword t;
  char c;
  t = g_kthreadInfo.tick;
  for(;;){
    if(g_kthreadInfo.tick > (t+timeout)){
      return false;
    }
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }
    if(c & 0x21){
      return false;
    }
    return true;
  }
  return false;
}

bool IDEDriver::waitdata(unsigned long timeout){
  dword t,k;
  char c;
  t = g_kthreadInfo.tick;
  for(;;){
    k = g_kthreadInfo.tick;
    if(k > (t+timeout)){
      return false;
    }
    c = inportb(status_);
    if(c & 0x80){
      return true;
    }
  }
  return false;
}

bool IDEDriver::waitready(unsigned long timeout){
  dword t;
  char c;
  t = g_kthreadInfo.tick;
  for(;;){
    if(g_kthreadInfo.tick > (t+timeout)){
      return false;
    }
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }
    
    if(c & 0x40){
      return true;
    }
  }
  return false;
}


bool IDEDriver::sendcmd(int cmd,byte *bfr,int bfrsize/* must be 2n size */){
  dword t;
  char c;
  waitready(TIMEOUT);
  outportb(status_,cmd);
  t = g_kthreadInfo.tick;
  for(;;){
    if(g_kthreadInfo.tick > (t + TIMEOUT_CMD)){
      return false;
    }
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }
    if(c & 0x21){
      return false;
    }
    if(c & 0x40){
      break;
    }
  }
  for(t=0;t!=bfrsize;t+=2){
    int a;
    asm volatile ("in %%dx, %%ax": "=a"(a): "d"(data_));
    bfr[t] = a >> 8;
    bfr[t+1] = a & 0xff;
  }
  return true;
}

bool IDEDriver::cmdRead(byte *bfr,unsigned int count){
  dword t;
  long bfrsize;
  char c;
  waitready(TIMEOUT);
  setCount(count);
  bfrsize = count * 512;
  outportb(status_,IDE_CMD_READ); /* use PIO read */
  t = g_kthreadInfo.tick;
  for(;;){
    if(g_kthreadInfo.tick > (t + TIMEOUT_CMD)){
      return false;
    }
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }
    if(c & 0x21){
      return false;
    }
    if(c & 0x40){
      break;
    }
  }
  for(t=0;t!=bfrsize;t+=2){
    int a;
    asm volatile ("in %%dx, %%ax": "=a"(a): "d"(data_));
    bfr[t+1] = a >> 8;
    bfr[t] = a & 0xff;
  }
  return true;
}

bool IDEDriver::senddevice(int drive){
  char c;
  c = inportb(head_);
  if(drive == 1){
    c |= 0x10;
  }else if(drive == 0){
    c &= 0xef;
  }
  outportb(head_,c);
  waithdc(TIMEOUT);
  return true;
}
IDEDriver::~IDEDriver() {
    
    return;
}

bool IDEDriver::initilize() {
    /* send software reset*/
    char c;
    console_->printf("ide:init:");

    waithdc(TIMEOUT);
    
    HasMaster = true;
    HasSlave = true;
    console_->printf("diag skipped.\n");
    
    /*
    console_->printf("diag...");
    if(sendcmd(IDE_CMD_DIAG,0,0)){
      c = inportb(error_);
      if(c & 0x0e){
        console_->printf("error.");
        HasMaster = false;
        HasSlave = false;
        return false;
      }else{
        console_->printf("ok.");
        HasMaster = true;
      }
      
      if(c & 0x80){
        console_->printf("(slave device)\n");
        HasSlave = false;
      }else{
        console_->printf("(slave device ok or not present)\n");
        HasSlave = true;
      }
    }else{
      console_->printf("no responce...(emulator?)\n");
      HasMaster = true;
      HasSlave = true;
    }
    */
    
    if(HasMaster){
      Master = new IDEDevice(this,0);
    }
    
    if(HasSlave){
      Slave = new IDEDevice(this,1);
    }
    
    console_->printf("\n");
    return true;
}

bool IDEDriver::setLBA(dword lba,unsigned int device){
  IDEDevice *d;
  char c;
  if((device == 0) && HasMaster ){
    d = Master;
  }else if((device == 1) && HasSlave){
    d = Slave;
  }else{
    return false;
  }
  if(d->IsSurpportLBA){
    c = ( lba >> 24 ) & 0xff;
    c &= 0x0f;
    c |= 0x40; /* LBA mode */
    if(device != 0){
      c |= 0x10;
    }
    outportb(head_,c);
    
    c = ( lba >> 16 ) & 0xff;
    outportb(cylinderH_,c);
    
    c = ( lba >> 8 ) & 0xff;
    outportb(cylinderL_,c);
    
    c = lba & 0xff;
    outportb(sector_,c);
  }else{
    return false;
  }
  return true;
}

void IDEDriver::setCount(byte count){
  outportb(count_,count);
}

unsigned int IDEDevice::device_;
unsigned int IDEDevice::SectorsPerTrack;
unsigned int IDEDevice::Tracks;
unsigned int IDEDevice::Heads;
IDEDriver* IDEDevice::Bus;
unsigned int IDEDevice::BytesPerSector;
unsigned int IDEDevice::TotalSize;
bool IDEDevice::IsSurpportLBA;

IDEDevice::IDEDevice(IDEDriver *bus,unsigned int device){
    byte buf[512];
    int i;
    device_ = device;
    Bus = bus;
    Bus->console_->printf("detecting ");
    if( !Bus->senddevice(device_)){
      if(device == 0){
        Bus->console_->printf("Master couldn't select\n");
        Bus->HasMaster = false;
        return;
      }else if(device == 1){
        Bus->console_->printf("Slave  couldn't select\n");
        Bus->HasSlave = false;
        return;
      }
    }   
    if(device == 0){
      Bus->console_->printf("Master:");
    }else if(device == 1){
      Bus->console_->printf("Slave :");
    }
    if(Bus->sendcmd(IDE_CMD_ID,buf,256)){
      Bus->console_->printf(" Detected.\n");
      if(device == 0){
        Bus->HasMaster = true;
      }else if(device == 1){
        Bus->HasSlave = true;
      }
      Bus->console_->printf("Name   : ");
      for(i=0;i!=40;i++){
        Bus->console_->printf("%c",buf[0x1b*2+i]);
      }
      Bus->console_->printf("\nVersion: ");
      for(i=0;i!=8;i++){
        Bus->console_->printf("%c",buf[0x17*2+i]);
      }
      Heads = (buf[0x03*2] << 8) + buf[0x03*2+1];
      Tracks = (buf[0x01*2] << 8) + buf[0x01*2+1];
      SectorsPerTrack = (buf[0x06*2] << 8) + buf[0x06*2+1];
      BytesPerSector = 512;
      IsSurpportLBA = buf[49*2] & 2;
      if(IsSurpportLBA){
        TotalSize = ( ( buf[60*2] << 8 ) + buf[60*2+1] )+( ( ( buf[61*2] << 8 ) + buf[61*2+1] ) << 16);
        Bus->console_->printf("\ntotalize = %d size = %d MB(LBA Device).\n",TotalSize,TotalSize/2/1024);
        Bus->console_->printf("C/H/S = %d/%d/%d (Mona won't use this info.)\n",Tracks,Heads,SectorsPerTrack);
      }else{
        Bus->console_->printf("\nC/H/S = %d/%d/%d sectorsize = %d size = %d MB (non-LBA Device)\n",Tracks,Heads,SectorsPerTrack,BytesPerSector,Heads*Tracks*SectorsPerTrack/1024*BytesPerSector/1024);
      }
      if(read(0,buf)){ /* test routine */
        /* dump */
        /*
        int j;
        char c;
        for(i=0;i!=512;i+=16){
          Bus->console_->printf("\n");
          Bus->console_->printf("%x : ",i);
          for(j=0;j!=16;j++){
            c = buf[i + j] >> 4;
            if(c > 9){
              c += ('A' - 10);
            }else{
              c += '0';
            }
            Bus->console_->printf("%c",c);
            c = buf[i + j] & 0x0f;
            if(c > 9){
              c += ('A' - 10);
            }else{
              c += '0';
            }
            Bus->console_->printf("%c ",c);
            
          }
        }
        */
        for(i=0;i!=4;i++){
          dword d;
          byte *mbr;
          mbr = buf + 0x1be + i*16;
          d = *(dword *)(mbr +8);
          Bus->console_->printf("\n");
          Bus->console_->printf("%d: ",i);
          if(!d){
            Bus->console_->printf("(Not found)");
            continue;
          }
          if(mbr[0] == 0x80){
            Bus->console_->printf("Boot ");
          }else{
            Bus->console_->printf("     ");
          }
          d = *(dword *)(mbr +8);
          Bus->console_->printf("from:%x ",d);
          d = *(dword *)&mbr[12];
          Bus->console_->printf("size:%x (%d MB) fs:",d,d/2/1024);
          switch(mbr[4]){ /* see also:http://www37.tok2.com/home/nobusan/partition/partition.html */
            case 0x01:
              Bus->console_->printf("FAT12");
              break;
            case 0x04:
              Bus->console_->printf("FAT16(<32MB)");
              break;
            case 0x06:
              Bus->console_->printf("FAT16(>32MB)");
              break;
            case 0x0b:
              Bus->console_->printf("FAT32");
              break;
            case 0x0c:
              Bus->console_->printf("FAT32-LBA");
              break;
            case 0x0e:
              Bus->console_->printf("FAT16-LBA");
              break;
            case 0x07:
              Bus->console_->printf("NTFS/HPFS");
              break;
            case 0x05:
              Bus->console_->printf("extend");
              break;
            case 0x0f:
              Bus->console_->printf("extend-LBA");
              break;
            case 0x82:
              Bus->console_->printf("Linux SWAP/Solaris");
              break;
            case 0x83:
              Bus->console_->printf("extend-Linux");
              break;
            case 0x2c:
              Bus->console_->printf("clfs-one");
              break;
            default:
              Bus->console_->printf("(#%x)",mbr[4]);
          }
          
        }
        Bus->console_->printf("\n");
      }else{
        Bus->console_->printf("read failed.\n");
      }
    }else{
      
      Bus->console_->printf(" Not Found.\n");
      if(device == 0){
        Bus->HasMaster = false;
      }else if(device == 1){
        Bus->HasSlave = false;
      }
      
    }
    Bus->console_->printf("\n");
}

IDEDevice::~IDEDevice(){
}

bool IDEDevice::read(dword lba, byte* buf) {
    if(Bus->setLBA(lba,device_)){
      return Bus->cmdRead(buf,1);
    }else{
      return false;
    }
}


bool IDEDevice::write(dword lba, byte* buf) {
    return true;
}


/*
LBA = ( (cylinder * heads_per_cylinder + heads )
        * sectors_per_track ) + sector - 1
*/
