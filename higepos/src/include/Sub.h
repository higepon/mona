#ifndef _HIGEPOS_SUB_
#define _HIGEPOS_SUB_

#include<Base.h>
/*!
    abstract class Sub for testing C++ environment
*/
class Sub : public Base {
  public:
      static Sub& instance() {
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("hlt");
          static Sub theInstance;
          return theInstance;
      }
      int   getNumber();
      char* getName();
 private:
      Sub();
};

#endif
