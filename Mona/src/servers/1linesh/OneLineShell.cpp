/*!
  \file   OneLineShell.cpp
  \brief  one line shell

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/
#include <monapi.h>
#include <monalibc.h>
#include <monapi/Keys.h>
#include "OneLineShell.h"
#include "DisplayWindow.h"
#include "Charing.h"
#include "Command.h"
#include "CommandHistory.h"

using namespace MonAPI;

OneLineShell::OneLineShell(){

  return;
}

OneLineShell::~OneLineShell(){

  return;
}

void OneLineShell::service() {

  /* look up */
  dword myID = System::getThreadID();

  dword targetID = Message::lookupMainThread("KEYBDMNG.SVR");
  if(targetID == 0xFFFFFFFF){
    printf("Shell:KeyBoardServer not found\n");
    exit(1);
  }

  /* create message for KEYBDMNG.SVR */
  MessageInfo info;
  Message::create(&info, MSG_KEY_REGIST_TO_SERVER, myID, 0, 0, NULL);

  /* send */
  if(Message::send(targetID, &info)){
      printf("Shell: key regist error\n");
  }

  /* Server start ok */
  targetID = Message::lookupMainThread("INIT");
  if(targetID == 0xFFFFFFFF){
    targetID = Message::lookupMainThread("SHELL.SVR");
    if(targetID == 0xFFFFFFFF){
      printf("ShellServer:INIT not found\n");
      exit(1);
    }
  }

  /* create message */
  Message::create(&info, MSG_SERVER_START_OK, 0, 0, 0, NULL);

  /* send */
  if(Message::send(targetID, &info)){
    printf("ShellServer:INIT error\n");
  }

  DisplayWindow ds;
  ds.DrawCommandWindow();
  this->cmdHst.AddCommand(this->cmd);
  /* service loop */
  while(1){
    if(!Message::receive(&info)){
      if((info.arg2 & KEY_MODIFIER_DOWN)){
        KeyInfo keyInfo;
        keyInfo.keycode = info.arg1;
        keyInfo.modifiers = info.arg2;
        keyInfo.charcode = info.arg3;
        this->OnKeyDown(keyInfo);
        ds.DrawCommandLine((char *)this->cmd);
        ds.DrawCursor(this->cmd.GetCurrentPos());
        ds.DrawMessageLine(this->msg);
      }
    }
  }
  return;
}

int OneLineShell::OnKeyDown(KeyInfo keyInfo){
  
  Charing *cTmp;

  switch(keyInfo.keycode){
  case Keys::Enter:
    cTmp = (Charing *)this->cmd;
    if(cTmp->GetLength() == 0) break;
    this->SetMessage(this->cmd.ExecuteCommand());
    cTmp = (Charing *)(this->cmdHst.GetCommand(GETLAST));
    if(cTmp->GetLength() == 0){
      this->cmdHst.UpdateHistory(this->cmd);
    } else {
      this->cmdHst.AddCommand(this->cmd);
    }
    this->cmd.InitializeCommandLine();
    this->cmdHst.AddCommand(this->cmd);
    break;
  case Keys::Back:
    this->cmd.RemoveCommandLine();
    break;
  case Keys::Right:
    this->cmd.SetCurrentPos(POSITION_RIGHT);
    break;
  case Keys::Left:
    this->cmd.SetCurrentPos(POSITION_LEFT);
    break;
  case Keys::Up:
    this->cmdHst.UpdateHistory(this->cmd);
    this->cmd = this->cmdHst.GetCommand(GETPREV);
    break;
  case Keys::Down:
    this->cmdHst.UpdateHistory(this->cmd);
    this->cmd = this->cmdHst.GetCommand(GETNEXT);
    break;
  default:
    if(keyInfo.modifiers & KEY_MODIFIER_CHAR){
      this->cmd.InsertCommandLine(keyInfo.charcode);
    }
    break;
  }
  return 0;
}

void OneLineShell::SetMessage(Charing message){

  this->msg = message;
  return;
}

void OneLineShell::SetMessage(int nMsg){

  Charing tmpMsg;

  switch(nMsg){
    case Excute:
      tmpMsg = "Command excute";
      break;
    case NothingFile:
      tmpMsg = "File not found";
      break;
    case MemErr:
      tmpMsg = "load Process memory allocate error";
      break;
    case FileReadErr:
      tmpMsg = "File read error";
      break;
    case SMemErr1:
      tmpMsg = "Shared Memory error1";
      break;
    case SMemErr2:
      tmpMsg = "Shared Memory error2";
      break;
    default:
      break;
  }

  this->msg = tmpMsg;;

  return;
}
