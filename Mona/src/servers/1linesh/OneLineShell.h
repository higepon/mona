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
#include "Command.h"
#include "CommandHistory.h"

using namespace MonAPI;

class OneLineShell : public Server{

  private:
    Command cmd;
    CommandHistory cmdHst;

  public:
    OneLineShell();
    virtual ~OneLineShell();
    void service();
    int OnKeyDown(int keycode, int modifiers);

};

#endif
