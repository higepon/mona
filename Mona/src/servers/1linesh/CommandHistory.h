/*!
  \file   CommandHistory.h
  \brief  one line shell command history

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _COMAND_HISTORY_
#define _COMAND_HISTORY_
#include <sys/types.h>
#include <monapi/CString.h>
#include "Command.h"

#define HISTORYSIZE 32

#define GETCURRENT 0
#define GETPREV   -1
#define GETNEXT   -2
#define GETLAST   -3
#define GETFIRST  -4

typedef struct tCommandHistory{
  struct tCommandHistory *prev;
  struct tCommandHistory *next;
  Command command;
} commandHistory_t;

/*!
  \brief Command Hittory class
*/
class CommandHistory{

  private:
    int historySize;
    commandHistory_t history;
    commandHistory_t *historyCurrent;
    commandHistory_t *SearchHistory(int numIndex);

  public:
    CommandHistory();
    ~CommandHistory();
    bool AddCommand(Command cmd);
    Command GetCommand(int numIndex);
    CString GetHistoryInfo(int numIndex, unsigned int numSize);
    bool UpdateHistory(Command cmd);
    int RemoveHistory(int numIndex, unsigned int numSize);

};

#endif
