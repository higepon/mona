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
#include <monapi/keys.h>
#include "OneLineShell.h"
//#include "KeyBoardManager.h"
#include "DisplayWindow.h"
#include "Charing.h"
#include "Command.h"

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
    printf("ShellServer:INIT not found\n");
    exit(1);
  }

  /* create message */
  Message::create(&info, MSG_SERVER_START_OK, 0, 0, 0, NULL);

  /* send */
  if(Message::send(targetID, &info)){
    printf("ShellServer:INIT error\n");
  }
    
  DisplayWindow ds;
  ds.DrawCommandWindow();
  /* service loop */
  while(1){
    if(!Message::receive(&info)){
      if((info.arg2 & KEY_MODIFIER_DOWN)){
        this->OnKeyDown(info.arg1, info.arg2);
        ds.DrawCommandLine((char *)this->cmd);
        ds.DrawCursor(this->cmd.GetCurrentPos());
      }
    }
  }
  return;
}

int OneLineShell::OnKeyDown(int keycode, int modifiers){
  
  KeyInfo keys;

  keys.keycode = keycode;
  keys.modifiers = modifiers;
  switch(keycode){
  case Keys::Enter:
    this->cmd.ExecuteCommand();
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
    //this->cmdHst.
    break;
  case Keys::Down:
    //printf("DOWN");//this->cmd.;??
    break;
  default:
    if(Keys::IsToChar(keys) == true){
      char c = Keys::ToChar(keys);
      this->cmd.InsertCommandLine(c);
    }
    break;

  }
  return 0;
}


