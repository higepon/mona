/*!
  \file   Command.cpp
  \brief  one line shell command

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/
#include <monapi.h>
#include <monapi/string.h>
#include <monapi/CString.h>
#include <monalibc/stdio.h>
#include <monapi/messages.h>
#include "Command.h"

using namespace MonAPI;

Command::Command(){

  this->posCurrent = 0;
  this->isTerminateFlag = false;

  return;
}

Command::~Command(){

  return;
}

int Command::ParseCommand(){

  return 0;
}

int Command::GetCurrentPos(){

  return this->posCurrent;
}

int Command::SetCurrentPos(int pos){

  if(pos == POSITION_LEFT){
    this->posCurrent -=1;
  } else if(pos == POSITION_RIGHT){
    this->posCurrent += 1;
  } else {
    this->posCurrent = pos;
  } 

  if(this->posCurrent < 0){
    this->posCurrent = 0;
  } else if(this->posCurrent > this->commandLine.getLength()){
    this->posCurrent = this->commandLine.getLength();
  }

  this->isTerminateFlag = false;
  return this->posCurrent;
}

bool Command::InsertCommandLine(const CString& s){

  this->commandLine.insert(this->posCurrent, s);
  this->posCurrent++;
  this->isTerminateFlag = false;
  return true;
}

bool Command::RemoveCommandLine(){

  if(this->posCurrent <= 0) return false;
  this->posCurrent--;
  this->isTerminateFlag = false;
  this->commandLine.remove(this->posCurrent, 1);
  return true;
}

bool Command::InitializeCommandLine(){

  this->isTerminateFlag = false;
  this->commandLine.remove(0, this->commandLine.getLength());
  this->posCurrent = 0;
  return true;
}

bool Command::IsTerminate(){

  return this->isTerminateFlag;
}

int Command::ExecuteCommand(){
  
  int result = 0;
  //CommandOption list;
  //list.next = NULL;
  //char* arg;
  //CommandOption* option;
/*
  char* command = strtok((char *)this->commandLine, " ");

  while ((arg = strtok(NULL, " ")) != NULL) {
    option = new CommandOption;
    strncpy(option->str, arg, 32);
    option->next = list.next;
    list.next = option;
  }*/

  char path[128];
  sprintf(path, "/APPS/%s", (const char *)this->commandLine);
/*
  result = syscall_load_process(path, command, &list);

  for (option = list.next; option; option = option->next) {
    delete option;
  }*/

  result = monapi_call_elf_execute_file(path, 0);

  this->SetCurrentPos(this->commandLine.getLength());
  this->isTerminateFlag = true;
  return result;
}

Command Command::operator+(const CString& s){

  this->commandLine += s;
  this->SetCurrentPos(this->commandLine.getLength());
  return *this;
}

Command& Command::operator=(const CString& s){

  this->commandLine = s;
  this->SetCurrentPos(this->commandLine.getLength());
  return *this;
}

Command& Command::operator+=(const CString& s){

  this->commandLine += s;
  this->SetCurrentPos(this->commandLine.getLength());
  return *this;
}

Command::operator CString *(){

  return &this->commandLine;
}

Command::operator const char *(){

  return (const char *)this->commandLine;
}

