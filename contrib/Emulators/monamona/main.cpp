/*!
  \file   main.cpp
  \brief  MONAmona based BF

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/06/08
  \author  shadow

  $Revision$
  $Date$
*/
#include <monapi.h>
#include "BfMain.h"

int main(int argc, char* argv[]) {

  char *str = "NNNnMNNNNOAaMnONNNNNNMAaOo MNNNNNNnONNNNNMAaOAo NNNNNNNoo NNNo MNNNNnMNNNNNNNNOAaMo nMNNNOAaMAAAAAAAAAo OOOo NNNoAAAAAAoAAAAAAAAo M";
  char *str2 = "NNNnMNNNNOAaMnONNNNNNMAaOo MNNNNNNnONNNNNMAaOAo NNNNNNNoo NNNo MNNNNnMNNNNNNNNOAaMo NNNNNNnMNNOAaMNo AAAnMNOAAaMnMNNNOAaMo Ao AAAAAAAAAAAAAo";
  BfMain *bfm = new BfMain();
  bfm->Parse(str);
  printf("\n");
  bfm->Init();
  bfm->Parse(str2);

  return 0;
}

