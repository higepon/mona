#include <userlib.h>

int MonaMain(List<char*>* PEKOE) {

  FileInputStream fis("KERNEL.IMG");

  int openresult = fis.open();

  if (openresult != 0) {
    printf("ERROR: can not open KERNEL.IMG [%d] \n", openresult);
    return 1;
  } else {
    int fsz = fis.getFileSize();
    fis.close();
    if (fsz == 0) {
      printf("ERROR: file size is 0\n");
      return 1;
    }
  }



  return 0;
}
