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
unsigned int IDEDriver::drive_;


IDEDriver::IDEDriver(VirtualConsole* console,unsigned int port,unsigned int drive) {

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
    drive_ = drive;
    
    initilize();
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



bool IDEDriver::sendcmd(int cmd,short int *bfr,int bfrsize){
  long t;
  char c;
  waitready(TIMEOUT);
  outportb(status_,cmd);
  for(;;){ /* long wait */
    c = inportb(status_);
    if(c & 0x80){
      continue;
    }/*
    if(!(c & 0x29)){
      continue;
    }*/
    if(c & 0x21){
      console_->printf("ide:sendcmd:device reports error..\n");
      return false;
    }
    if(c & 0x40){
      break;
    }
  }
  for(t=0;t!=bfrsize;t++){
    asm volatile ("in %%dx, %%ax": "=a"(bfr[t]): "d"(data_));
  }
  return true;
}
bool IDEDriver::senddrive(int drive){
  char c;
  c = inportb(head_);
  if(drive == 1){
    c |= 0x10;
  }else if(drive == 0){
    c &= 0xef;
  }else{
    console_->printf("ide:senddrive:unsurpported drive no.\n");
    return false;
  }
  outportb(head_,drive);
  waithdc(TIMEOUT);
  return true;
}
IDEDriver::~IDEDriver() {
    
    return;
}

void IDEDriver::initilize() {
    /* send software reset*/
    char c;
    short int buf[256];
    int i;
    console_->printf("ide:init:");
    waithdc(TIMEOUT);
    console_->printf("diag...");
    sendcmd(IDE_CMD_DIAG,0,0);
    c = inportb(error_);
    if(c & 0x0e){
      console_->printf("error.");
    }else{
      console_->printf("ok.");
    }
    if(c & 0x80){
      console_->printf("and there is NO slave drive.\n");
    }else{
      console_->printf("and there is slave drive.\n");
    }
    sendcmd(IDE_CMD_ID,buf,256);
    console_->printf("Name: ");
    for(i=0;i!=40;i+=2){
      console_->printf("%c",*((char *)&buf[0x1b]+i+1));
      console_->printf("%c",*((char *)&buf[0x1b]+i));
    }
    console_->printf("\nVersion: ");
    for(i=0;i!=8;i+=2){
      console_->printf("%c",*((char *)&buf[0x17]+i+1));
      console_->printf("%c",*((char *)&buf[0x17]+i));
    }
    console_->printf("\n");
    return;
}

bool IDEDriver::read(int lba, byte* buf) {
    return true;
}


bool IDEDriver::write(int lba, byte* buf) {
    return true;
}
