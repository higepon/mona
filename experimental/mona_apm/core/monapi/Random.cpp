/*----------------------------------------------------------------------
    Random
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Random::Random()
{
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);
}

Random::Random(unsigned long seed)
{
    init_genrand(seed);
}

Random::~Random()
{
}

int Random::nextInt()
{
    return (int)genrand_int31();
}

void Random::setSeed(unsigned long seed)
{
    init_genrand(seed);
}

double Random::nextDouble()
{
    return genrand_real1();
}

}
