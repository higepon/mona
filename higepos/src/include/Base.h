#ifndef _HIGEPOS_BASE_
#define _HIGEPOS_BASE_

/*!
    abstract class Base for testing C++ environment
*/
class Base {
  public:
    virtual int   getNumber(void) = 0;
    virtual char* getName(void)   = 0;
};

#endif
