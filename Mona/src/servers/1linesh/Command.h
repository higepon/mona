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
#include <monapi/CString.h>

#define POSITION_RIGHT -2
#define POSITION_LEFT -1

using namespace MonAPI;

/*!
  \brief Command class
*/
class Command{

  private:
    CString commandLine;
    int posCurrent;
    bool isTerminateFlag;
    
    int ParseCommand();

  public:
    Command();
    ~Command();
    int GetCurrentPos();
    int SetCurrentPos(int pos);
    bool InsertCommandLine(const CString& s);
    bool RemoveCommandLine();
    bool InitializeCommandLine();
    bool IsTerminate();
    int ExecuteCommand();

    Command operator+(const CString& s);
    Command& operator=(const CString& s);
    Command& operator+=(const CString& s);
    operator const char *();
    operator CString *();
};


#endif
