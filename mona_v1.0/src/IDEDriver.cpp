/* IDE */

#include<IDEDriver.h>
#include<kernel.h>
#include<operator.h>
#include<io.h>
#include<global.h>

/* IDE commands*/
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

#define TIMEOUT 0xffffffff
#define TIMEOUTMULT 90
bool IDEDriver::waithdc(unsigned long timeout){
  long t,r;
  char c;
  for(t = timeout;t!=0;t--){
    for(r = 0;r!=TIMEOUTMULT;r++);
    c = inportb(status_);
    if(!(c & 0x80)){
      return true;
    }
  }
  console_->printf("ide:waithdc:timeout.\n");
  return false;
}

bool IDEDriver::waitdata(unsigned long timeout){
  long t,r;
  char c;
  for(t = timeout;t!=0;t--){
    for(r = 0;r!=TIMEOUTMULT;r++);
    c = inportb(status_);
    if(c & 0x80){
      return true;
    }
  }
  console_->printf("ide:waithdc:timeout.\n");
  return false;
}

bool IDEDriver::waitready(unsigned long timeout){
  long t,r;
  char c;
  for(t = timeout;t!=0;t--){
    for(r = 0;r!=TIMEOUTMULT;r++);
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }
    
    if(c & 0x40){
      return true;
    }
  }
  console_->printf("ide:waithdc:timeout.\n");
  return false;
}



bool IDEDriver::sendcmd(int cmd,char *bfr,int bfrsize/* must be 2n size */){
  long t;
  char c;
  waitready(TIMEOUT);
  outportb(status_,cmd);
  for(;;){ /* long wait */
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }
    if(c & 0x21){
      console_->printf("ide:sendcmd:device reports error..\n");
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
    console_->printf("diag...");
    if(!sendcmd(IDE_CMD_DIAG,0,0)){
      console_->printf("\naborting...\n");
      HasMaster = false;
      return false;
    }
    c = inportb(error_);
    if(c & 0x0e){
      console_->printf("error.");
      HasMaster = false;
      return false;
    }else{
      console_->printf("ok.");
      HasMaster = true;
    }
    if(c & 0x80){
      console_->printf("and there is NO slave drive.\n");
      HasSlave = false;
    }else{
      console_->printf("and there is slave drive.\n");
      HasSlave = true;
    }
    if(HasMaster){
      Master = new IDEDevice(this,0);
    }
    if(HasSlave){
      Slave = new IDEDevice(this,1);
    }

    console_->printf("\n");
    return true;
}

unsigned int IDEDevice::device_;
IDEDriver* IDEDevice::Bus;
IDEDevice::IDEDevice(IDEDriver *bus,unsigned int device){
    char buf[256];
    int i;
    device_ = device;
    Bus = bus;
    Bus->senddevice(device_);
    bus->sendcmd(IDE_CMD_ID,buf,256);
    if(device == 0){
      Bus->console_->printf("Master:\n");
    }else if(device == 1){
      Bus->console_->printf("Slave:\n");
    }
    Bus->console_->printf("Name: ");
    for(i=0;i!=40;i++){
      Bus->console_->printf("%c",buf[0x1b*2+i]);
    }
    Bus->console_->printf("\nVersion: ");
    for(i=0;i!=8;i++){
      Bus->console_->printf("%c",buf[0x17*2+i]);
    }
    Bus->console_->printf("\n\n");
}

IDEDevice::~IDEDevice(){
}

bool IDEDevice::read(dword lba, byte* buf) {
    return true;
}


bool IDEDevice::write(dword lba, byte* buf) {
    return true;
}

