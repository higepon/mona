#include <stdio.h>

#include "IDManager.h"

#define OBJECT_NUM 10

int main(int argc, char *argv[])
{
    IDManager* im = new IDManager();

    KObject objects[OBJECT_NUM];
    int id[OBJECT_NUM];

    printf("initial set\n");
    for (int i = 0; i < OBJECT_NUM; i++)
    {
        id[i] = im->allocateID(&(objects[i]));
        printf("id = %d object = %x\n", id[i], &(objects[i]));
    }

    printf("get from IDManager\n");
    for (int i = 0; i < OBJECT_NUM; i++)
    {
        printf("id = %d object = %x\n", id[i], im->get(id[i], NULL));
    }

    printf("get from IDManager reverse\n");
    for (int i = OBJECT_NUM - 1; i >= 0; i--)
    {
        printf("id = %d object = %x\n", id[i], im->get(id[i], NULL));
    }

    printf("allocate double\n");
    int id2 = im->allocateID(&(objects[0]));
    printf("id2 = %d refcount = %d\n", id2, objects[0].getReferanceCount());
    im->returnID(id[0]);
    printf("return refcount = %d\n", objects[0].getReferanceCount());
    delete im;
    return 0;
}
