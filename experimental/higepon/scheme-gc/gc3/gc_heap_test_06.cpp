#include "GCRecord.h"
#include "../util/Vector.h"
#include "../util/Assert.h"
#include "../util/String.h"

extern GCRecord root;
using namespace util;

Vector<int*> v;

void test()
{
    v.add(new int);
}

int main(int argc, char *argv[])
{
    gc_init();
    test();
    gc();
    int size = gc_record_size(&root);
    if (size == 2
        && root.next->size == 4
        && root.next->next->size == 20)
    {
        printf("[OK] %s\n", argv[0]);
        return 0;
    }
    else
    {
        printf("%s [NG]\n", argv[0]);
        FOREACH_GC_RECORD(&root, e)
        {
            printf("    not sweeped size = %d\n", e->size);
        }
        return 1;
    }

}
