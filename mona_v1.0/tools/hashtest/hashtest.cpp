#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "collection.h"
#include "types.h"

typedef unsigned int dword;
typedef unsigned char byte;

/*
    main()
*/

#define RANDOMIZE() srand(time(NULL))
#define RANDOM(x) ((rand() % (x)) + 1)

char* strCopy(char* source) {

    char* p = (char*)malloc(strlen(source) + 1);
    strcpy(p, source);
    return p; /* memory leak oh yeah */
}

void addItem(List<int>* list, BinaryTree<int>* tree) {

    int key;

    for (;;) {
        key = RANDOM(9999999);
        if (list->hasElement(key)) {
            continue;
        } else {
            list->add(key);
            break;
        }
    }

    tree->add(key, key + 1);
}

void addItem(List<int>* list, Map<int>* map) {

   int key;
   char keybuf[512];

    for (;;) {
        key = RANDOM(9999999);
        if (list->hasElement(key)) {
            continue;
        } else {
            break;
        }
    }

    list->add(key);
    sprintf(keybuf, "%d", key);
    map->put(strCopy(keybuf), (key + 1));
}

void equal(const char* message, int x, int y) {

    if (x == y) {
        printf("OK:%s (%d, %d)\n", message, x, y);
        return;
    } else {
        printf("NG:%s (%d, %d)\n", message, x, y);
        exit(-1);
    }
}

void testBinaryTree() {

    RANDOMIZE();
    BinaryTree<int>* tree = new BinaryTree<int>;
    List<int>* list       = new HList<int>;

    /* add */
    for (int i = 0; i < 5000; i++) {
        addItem(list, tree);
    }

    /* number of elements */
    equal("number of elements", 5000, tree->size());

    /* check of add */
    for (int i = 0; i < list->size(); i++) {

        int key    = list->get(i);
        int result = tree->get(key);
        equal("element compare", key, result - 1);
    }

    /* remove all */
    for (int i = 0; i < list->size(); i++) {

        int key    = list->get(i);
        int result = tree->remove(key);
        if (result == 0) {
            equal("remove all ", 0, -1);
        }
    }

    /* check of remove */
    for (int i = 0; i < list->size(); i++) {

        int key    = list->get(i);
        equal("element get remove", 0, tree->get(key));
    }

    /* number of elements */
    equal("number of elements remove", 0, tree->size());

    delete list;
    list = new HList<int>;

    /* add */
    for (int i = 0; i < 5000; i++) {
        addItem(list, tree);
    }

    /* number of elements */
    equal("number of elements", 5000, tree->size());

    /* remove half */
    for (int i = 0; i < (list->size()) / 2; i++) {

        int key    = list->get(i);
        int result = tree->remove(key);
        if (result == 0) {
            equal("remove half ", 0, -1);
        }
    }

    /* check of remove */
    for (int i = 0; i < list->size() / 2; i++) {

        int key    = list->get(i);
        equal("element get remove2", 0, tree->get(key));
    }

    for (int i = list->size() / 2 + 1; i < list->size(); i++) {

        int key    = list->get(i);
        int result = tree->get(key);
        equal("element compare", key, result - 1);
    }

    delete list;
    delete tree;
}

int main(int argc, char** argv) {

    /*----------------------------------------------------------------------
        Binary Tree test
    ----------------------------------------------------------------------*/
    testBinaryTree();

    /*----------------------------------------------------------------------
        HashMap TEST
    ----------------------------------------------------------------------*/
    RANDOMIZE();
    char keybuf[128];
    Map<char*>* map  = new HashMap<char*>(512);

    map->put("/Mona/System/File", "file");
    map->put("/Mona/System/FDC", "FDCDriver");
    map->put("/Mona/System/Console", "GConsole");

    printf("%s\n", map->get("/Mona/System/File"));
    printf("%s\n", map->get("/Mona/System/FDC"));
    printf("%s\n", map->get("/Mona/System/Console"));

    map->put("AAAAB", "AAAAB");
    map->put("AAABA", "AAABA");
    map->put("AABAA", "AABAA");
    map->put("ABAAA", "ABAAA");

    printf("%s\n", map->get("AAAAB"));
    printf("%s\n", map->get("AAABA"));
    printf("%s\n", map->get("AABAA"));
    printf("%s\n", map->get("ABAAA"));
    delete map;
    return 0;
}
