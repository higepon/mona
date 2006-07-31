#ifndef _MONA_USERLIB_RANDOM_
#define _MONA_USERLIB_RANDOM_

#include <sys/types.h>

void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[],int key_length);
double genrand_real1(void);
unsigned long genrand_int31(void);

namespace MonAPI {

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
    double nextDouble();
    void setSeed(unsigned long seed);
};

}

#endif
