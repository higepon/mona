/*!
  \file   OneLineSh.h
  \brief  one line shell

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _ONE_LINE_SHELL_
#define _ONE_LINE_SHELL_
#include <sys/types.h>
#include <monapi.h>
#include <monapi/CString.h>
#include "Command.h"
#include "CommandHistory.h"

using namespace MonAPI;

class OneLineShell : public Server{

  private:
    Command cmd;
    CommandHistory cmdHst;
    CString msg;

  public:
    enum{
      Excute = 0,
      NothingFile = 1,
      MemErr = 2,
      FileReadErr = 3,
      SMemErr1 = 4,
      SMemErr2 = 5,
    };

    OneLineShell();
    virtual ~OneLineShell();
    void service();
    int OnKeyDown(KeyInfo keyInfo);
    void SetMessage(CString message);
    void SetMessage(int nMsg);

};

#endif
