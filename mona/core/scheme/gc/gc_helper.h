#include "GCRecord.h"
#include <stdio.h>
extern GCRecord root;

void show_gc_result(int expectedAsUsedBlock[], int usedNum, const char* name);
bool has_gc_error(int usedBlock[], int num);
void show_gc_error(int usedBlock[], int num);
