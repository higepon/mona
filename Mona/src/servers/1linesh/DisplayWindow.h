/*!
  \file   DisplayWindow.h
  \brief  display one line shell window

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/04/04
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _DISPLAY_WINDOW_
#define _DISPLAY_WINDOW_

#include <monapi.h>
#include <sys/types.h>
#include "Charing.h"
#include "Command.h"

using namespace MonAPI;

class DisplayWindow{

  private:
    int posX;
    int posY;
    int width;
    int height;
    int mode;
    Screen commandWindow;

  public:
    DisplayWindow();
    ~DisplayWindow();
    int SetWindowPos(int x, int y, int w, int h);
    int DrawCommandLine(const Charing c);
    int ClearCommandLine();
    int DrawCommandWindow();
    int ClearCommandWindow();
    int DrawCursor(int pos);

};

#endif
