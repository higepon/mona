/* pcic */
#ifndef __EXPR__PCIC_H
#define __EXPR__PCIC_H
#include <driver/ISADriver.h>
class PCIC {
  public:
    PCIC(ISADriver *isa);
    ~PCIC();
};

#endif
