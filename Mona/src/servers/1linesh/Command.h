/*!
  \file   Command.h
  \brief  one line shell command

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _COMAND_
#define _COMAND_
#include <sys/types.h>
#include "Charing.h"

#define POSITION_RIGHT -2
#define POSITION_LEFT -1

/*!
  \brief Command class
*/
class Command{

  private:
    Charing commandLine;
    int posCurrent;
    bool isTerminateFlag;
    
    int ParseCommand();

  public:
    Command();
    ~Command();
    int GetCurrentPos();
    int SetCurrentPos(int pos);
    bool InsertCommandLine(const Charing& s);
    bool RemoveCommandLine();
    bool IsTerminate();
    int ExecuteCommand();

    Command operator+(const Charing& s);
    Command& operator=(const Charing& s);
    Command& operator+=(const Charing& s);
    operator char *();
    operator Charing *();
};


#endif
