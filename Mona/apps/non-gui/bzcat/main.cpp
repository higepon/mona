#include <monapi.h>
#include <bzlib.h>

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    bz_stream bzs;
    bzs.bzalloc = NULL;
    bzs.bzfree  = NULL;
    printf("BZ2_bzDecompressInit: %d\n", BZ2_bzDecompressInit(&bzs, 0, 0));
    return 0;
}
