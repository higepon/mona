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
#include <monapi/messages.h>
#include <monapi/Keys.h>
#include <monapi/CString.h>
#include "OneLineShell.h"
#include "DisplayWindow.h"
#include "Command.h"
#include "CommandHistory.h"

using namespace MonAPI;

bool hasExited = false;

OneLineShell::OneLineShell(){

  return;
}

OneLineShell::~OneLineShell(){

  return;
}

void OneLineShell::service() {

  if(!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_TRUE)){
    printf("Shell: key regist error\n");
    exit(1);
  }

  /* Server start ok */
  dword targetID = Message::lookupMainThread("INIT");
  if(targetID == THREAD_UNKNOWN){
    targetID = MonAPI::System::getParentThreadID();
    if(targetID == THREAD_UNKNOWN){
      printf("ShellServer:INIT not found\n");
      exit(1);
    }
  }

  /* send */
  if(Message::send(targetID, MSG_SERVER_START_OK)){
    printf("ShellServer:INIT error\n");
  }

  DisplayWindow ds;
  ds.DrawCommandWindow();
  this->cmdHst.AddCommand(this->cmd);
  /* service loop */
  MessageInfo info;
  while(!hasExited){
    if(!Message::receive(&info)){
      if((info.arg2 & KEY_MODIFIER_DOWN)){
        KeyInfo keyInfo;
        keyInfo.keycode = info.arg1;
        keyInfo.modifiers = info.arg2;
        keyInfo.charcode = info.arg3;
        this->OnKeyDown(keyInfo);
        ds.DrawCommandLine((const char *)this->cmd);
        if(!hasExited) ds.DrawCursor(this->cmd.GetCurrentPos());
        ds.DrawMessageLine(this->msg);
      }
    }
  }

  if(!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_FALSE)){
      printf("Shell: key unregist error\n");
  }

  return;
}

int OneLineShell::OnKeyDown(KeyInfo keyInfo){
  
  CString *cTmp;

  switch(keyInfo.keycode){
  case Keys::Enter:
    cTmp = (CString *)this->cmd;
    if(cTmp->getLength() == 0) break;
    if(strcmp(*cTmp, "CHSH") == 0 || strcmp(*cTmp, "chsh") == 0){
      int result = monapi_call_process_execute_file("/SERVERS/OLDSHELL.EX2", MONAPI_TRUE);
      if(result != 0){
        this->SetMessage(result);
      } else {
        for(MessageInfo msg;;){
          if(Message::receive(&msg) != 0) continue;
          if(msg.header == MSG_SERVER_START_OK) break;
        }
        hasExited = true;
        this->SetMessage("Change shell to OLDSHELL.EX2");
      }
    } else {
      this->SetMessage(this->cmd.ExecuteCommand());
    }
    cTmp = (CString *)(this->cmdHst.GetCommand(GETLAST));
    if(cTmp->getLength() == 0){
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
      this->cmd.InsertCommandLine(CString((char *)&keyInfo.charcode, 1));
    }
    break;
  }
  return 0;
}

void OneLineShell::SetMessage(CString message){

  this->msg = message;
  return;
}

void OneLineShell::SetMessage(int nMsg){

  CString tmpMsg;

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
      if(nMsg == -1) tmpMsg = "Error from FILE.BIN or ELF.BIN";
      break;
  }

  this->msg = tmpMsg;;

  return;
}
