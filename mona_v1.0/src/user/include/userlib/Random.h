#ifndef _MONA_USERLIB_RANDOM_
#define _MONA_USERLIB_RANDOM_

#include <types.h>

void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[],int key_length);
unsigned long genrand_int31(void);
/*----------------------------------------------------------------------
    Random
----------------------------------------------------------------------*/
class Random
{
public:
    Random();
    Random(unsigned long seed);
    virtual ~Random();

public:
    int nextInt();
    void setSeed(unsigned long seed);
};

#endif
