/* IDE

    Copyright (c) 2003 .mjt
    All rights reserved.
    License=MIT/X Licnese

*/

#include<IDEDriver.h>
#include<kernel.h>
#include<operator.h>
#include<io.h>
#include<global.h>
#include<info.h>

#ifndef NOTICE
#define NOTICE (DEV_ERROR +1)
#endif

/* IDE commands*/
#define IDE_CMD_READ 0x20
#define IDE_CMD_DIAG 0x90
#define IDE_CMD_ID 0xec

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

IDEDriver::IDEDriver(unsigned int port) {

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
      info(NOTICE,"constructor:can't find IDE I/F in Port:%x\n",port);
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
  int t;
  char c;
  waitready(TIMEOUT);
  outportb(status_,cmd);
  t = g_kthreadInfo.tick;
  for(;;){
    if(g_kthreadInfo.tick > (dword)(t + TIMEOUT_CMD)){
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
  int t;
  long bfrsize;
  char c;
  waitready(TIMEOUT);
  setCount(count);
  bfrsize = count * 512;
  outportb(status_,IDE_CMD_READ); /* use PIO read */
  t = g_kthreadInfo.tick;
  for(;;){
    if(g_kthreadInfo.tick > (dword)(t + TIMEOUT_CMD)){
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
    info(NOTICE,"init...");

    waithdc(TIMEOUT);

    HasMaster = true;
    HasSlave = true;
    info(WARNING,"diag skipped.\n");

    /*
    info(DUMP,"diag...");
    if(sendcmd(IDE_CMD_DIAG,0,0)){
      c = inportb(error_);
      if(c & 0x0e){
        info(DUMP,"error.");
        HasMaster = false;
        HasSlave = false;
        return false;
      }else{
        info(DUMP,"ok.");
        HasMaster = true;
      }

      if(c & 0x80){
        info(DUMP,"(slave device)\n");
        HasSlave = false;
      }else{
        info(DUMP,"(slave device ok or not present)\n");
        HasSlave = true;
      }
    }else{
      info(DUMP,"no responce...(emulator?)\n");
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

    info(NOTICE,"\n");
    return true;
}

bool IDEDriver::setLBA(dword lba,unsigned int device){
/* ToDo:Length Check */
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
    c |= 0xa0;
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
    return setCHS(lba,device);
  }
  return true;
}

bool IDEDriver::setCHS(dword lba,unsigned int device){
  IDEDevice *d;
  char c;
  int T;
  int H;
  int S;
  if((device == 0) && HasMaster ){
    d = Master;
  }else if((device == 1) && HasSlave){
    d = Slave;
  }else{
    return false;
  }

/* from FDCDriver.cpp:
    track = lba / (2 * 18);
    head = (lba / 18) % 2;
    sector = 1 + lba % 18;
    return;
*/
  T = lba / (d->Heads * d->SectorsPerTrack );
  H = ( lba / d->SectorsPerTrack ) % d->Heads;
  S = 1 + ( lba % d->SectorsPerTrack );

  c = H;
  c |= 0xa0;
  outportb(head_,c);

  c = ( T >> 8 ) & 0xff;
  outportb(cylinderH_,c);

  c = ( T & 0xff);
  outportb(cylinderL_,c);

  c = S;
  outportb(sector_,c);
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
    info(NOTICE,"detecting ");
    if( !Bus->senddevice(device_)){
      if(device == 0){
        info(WARNING,"Master couldn't select\n");
        Bus->HasMaster = false;
        return;
      }else if(device == 1){
        info(WARNING,"Slave  couldn't select\n");
        Bus->HasSlave = false;
        return;
      }
    }
    if(device == 0){
      info(NOTICE,"Master:");
    }else if(device == 1){
      info(NOTICE,"Slave :");
    }
    if(Bus->sendcmd(IDE_CMD_ID,buf,256)){
      info(NOTICE," Detected.\n");
      if(device == 0){
        Bus->HasMaster = true;
      }else if(device == 1){
        Bus->HasSlave = true;
      }
      info(NOTICE,"Name   : ");
      for(i=0;i!=40;i++){
        info(NOTICE,"%c",buf[0x1b*2+i]);
      }
      info(NOTICE,"\n");
			info(NOTICE,"Version: ");
      for(i=0;i!=8;i++){
        info(NOTICE,"%c",buf[0x17*2+i]);
      }
      Heads = (buf[0x03*2] << 8) + buf[0x03*2+1];
      Tracks = (buf[0x01*2] << 8) + buf[0x01*2+1];
      SectorsPerTrack = (buf[0x06*2] << 8) + buf[0x06*2+1];
      BytesPerSector = 512;
      IsSurpportLBA = buf[49*2] & 2;
      info(NOTICE,"\n");
			if(IsSurpportLBA){
        TotalSize = ( ( buf[60*2] << 8 ) + buf[60*2+1] )+( ( ( buf[61*2] << 8 ) + buf[61*2+1] ) << 16);
        info(NOTICE,"totalize = %d size = %d MB(LBA Device).\n",TotalSize,TotalSize/2/1024);
        info(NOTICE,"C/H/S = %d/%d/%d (Mona won't use this info.)\n",Tracks,Heads,SectorsPerTrack);
      }else{
        info(NOTICE,"\nC/H/S = %d/%d/%d sectorsize = %d size = %d MB (non-LBA Device)\n",Tracks,Heads,SectorsPerTrack,BytesPerSector,Heads*Tracks*SectorsPerTrack/1024*BytesPerSector/1024);
      }
      if(read(0,buf)){ /* test routine */
        /* dump */
        /*
        int j;
        char c;
        for(i=0;i!=512;i+=16){
          info(DUMP,"\n");
          info(DUMP,"%x : ",i);
          for(j=0;j!=16;j++){
            c = buf[i + j] >> 4;
            if(c > 9){
              c += ('A' - 10);
            }else{
              c += '0';
            }
            info(DUMP,"%c",c);
            c = buf[i + j] & 0x0f;
            if(c > 9){
              c += ('A' - 10);
            }else{
              c += '0';
            }
            info(DUMP,"%c ",c);

          }
        }
        */
        for(i=0;i!=4;i++){
          dword d;
          byte *mbr;
          mbr = buf + 0x1be + i*16;
          d = *(dword *)(mbr +8);
          info(NOTICE,"\n");
          info(NOTICE,"%d: ",i);
          if(!d){
            info(NOTICE,"(Not found)");
            continue;
          }
          if(mbr[0] == 0x80){
            info(NOTICE,"Boot ");
          }else{
            info(NOTICE,"     ");
          }
          d = *(dword *)(mbr +8);
          info(NOTICE,"from:%x ",d);
          d = *(dword *)&mbr[12];
          info(NOTICE,"size:%x (%d MB) fs:",d,d/2/1024);
          switch(mbr[4]){ /* see also:http://www37.tok2.com/home/nobusan/partition/partition.html */
            case 0x01:
              info(NOTICE,"FAT12");
              break;
            case 0x04:
              info(NOTICE,"FAT16(<32MB)");
              break;
            case 0x06:
              info(NOTICE,"FAT16(>32MB)");
              break;
            case 0x0b:
              info(NOTICE,"FAT32");
              break;
            case 0x0c:
              info(NOTICE,"FAT32-LBA");
              break;
            case 0x0e:
              info(NOTICE,"FAT16-LBA");
              break;
            case 0x07:
              info(NOTICE,"NTFS/HPFS");
              break;
            case 0x05:
              info(NOTICE,"extend");
              break;
            case 0x0f:
              info(NOTICE,"extend-LBA");
              break;
            case 0x82:
              info(NOTICE,"Linux SWAP/Solaris");
              break;
            case 0x83:
              info(NOTICE,"extend-Linux");
              break;
            case 0x2c:
              info(NOTICE,"clfs-one");
              break;
            default:
              info(NOTICE,"(#%x)",mbr[4]);
          }

        }
        info(NOTICE,"\n");
      }else{
        info(ERROR,"read failed.\n");
      }
    }else{
      info(WARNING," Not Found.\n");
      if(device == 0){
        Bus->HasMaster = false;
      }else if(device == 1){
        Bus->HasSlave = false;
      }

    }
    /* READ TEST */ /*
    if(IsSurpportLBA){
      info(DUMP,"\nDevice read test...\n");
      dword tests;
      int cmpl;
      byte buf1[512];
      byte buf2[512];
      for(tests = 0; ( tests != TotalSize ) && (g_demo_step < 3) ; tests++){

        read(tests,buf1);
        IsSurpportLBA = false;
        read(tests,buf2);
        for(cmpl=0;cmpl!=512;cmpl++){
          if(buf1[cmpl] != buf2[cmpl]){
            info(DUMP,"READ ERROR IN %d @ %d !!",tests,cmpl);
            for(;;);
          }
        }
        IsSurpportLBA = true;
      }
      g_demo_step = 3;
    }
    */ /* ~READ TEST */
    info(NOTICE,"\n");
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
