#include <stdlib.h>
#include <stdio.h>

#include "collection.h"
#include "types.h"

typedef unsigned int dword;
typedef unsigned char byte;

/*
    main()
*/
int main(int argc, char** argv) {

    BinaryTree<char*>* tree = new BinaryTree<char*>;

    /* add */
    tree->add(1000, "1000");
    tree->add(1001, "1001");
    tree->add(1002, "1002");
    tree->add(1003, "1003");
    tree->add(1004, "1004");
    tree->add(1005, "1005");

    /* ok? */
    printf("tree size = %d\n", tree->size());
    for (dword  i = 1000; i < 1006; i++) {
        printf("i=[%s]\n", tree->get(i));
    }

    delete tree;
    return 0;
}
