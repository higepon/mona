#include <monapi.h>
#include "OneLineShell.h"

int MonaMain(List<char*>* pekoe) {

    OneLineShell* shell = new OneLineShell();
    shell->service();

    return 0;
}
