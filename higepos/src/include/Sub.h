#ifndef _HIGEPOS_SUB_
#define _HIGEPOS_SUB_

#include<Base.h>
/*!
    abstract class Sub for testing C++ environment
*/
class Sub : virtual Base {
  public:
      static Sub& instance() {
          static Sub theInstance;
          return theInstance;
      }
      int   getNumber();
      char* getName();
};

#endif
