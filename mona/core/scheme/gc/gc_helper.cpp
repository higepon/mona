#include "gc_helper.h"

void show_gc_result(int expectedAsUsedBlock[], int usedNum, const char* name)
{
    int size = gc_record_size(&root);
    if (has_gc_error(expectedAsUsedBlock, usedNum))
    {
        fprintf(stderr, "*** gc error *** %s\n", name);
        show_gc_error(expectedAsUsedBlock, usedNum);
    }
    else
    {
        if (size == usedNum)
        {
            printf("[OK] %s\n", name);
        }
        else
        {
            printf("[OK] : %s [warning]\n", name);
            FOREACH_GC_RECORD(&root, e)
            {
                printf("warning not sweeped size = %d\n", e->size);
            }
        }
    }
}

bool has_gc_error(int usedBlock[], int num)
{
    FOREACH_GC_RECORD(&root, e)
    {
        int i;
        for (i = 0; i < num; i++)
        {
            if (-1 == usedBlock[i]) continue;

            if (e->size == usedBlock[i])
            {
                usedBlock[i] = -1;
            }
        }
    }
    for (int i = 0; i < num; i++)
    {
        if(usedBlock[i] != -1)
        {
            return true;
        }
    }
    return false;
}

void show_gc_error(int usedBlock[], int num)
{
    for (int i = 0; i < num; i++)
    {
        if(usedBlock[i] != -1)
        {
            fprintf(stderr, "block(%d byte) disappear\n", usedBlock[i]);
        }
    }
}
