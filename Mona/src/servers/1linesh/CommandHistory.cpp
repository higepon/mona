/*!
  \file   CommandHistory.cpp
  \brief  one line shell command history

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/

#include "CommandHistory.h"

CommandHistory::CommandHistory(){

  this->historySize = 0;
  this->history.prev = &this->history;
  this->history.next = &this->history;
  this->historyCurrent = &this->history;

  return;
}

CommandHistory::~CommandHistory(){

  this->RemoveHistory(1, HISTORYSIZE-1);
  return;
}

commandHistory_t *CommandHistory::SearchHistory(int numIndex){

  int i, tmpIndex;
  commandHistory_t *tmp;
  
  if(numIndex < 0){
    tmpIndex = this->historySize + numIndex +1;
  } else {
    tmpIndex = numIndex;
  }

  if(numIndex == 0){
    tmp = this->historyCurrent;
  } else if(numIndex > this->historySize){
    tmp = NULL;
  } else {
    tmp = this->history.next;
    for(i = 1; i < numIndex; i++){
      tmp = tmp->next;
    }
  }
  return tmp;
}

bool CommandHistory::AddCommand(Command cmd){

  commandHistory_t *tmp;
  
  tmp = new commandHistory_t;

  tmp->prev = this->history.prev; /*add last*/
  tmp->next = &this->history;
  tmp->command = cmd;
  (tmp->prev)->next = tmp;
  this->history.prev = tmp;

  this->historyCurrent = tmp;
  this->historySize++;
  if(this->historySize > HISTORYSIZE){
    this->RemoveHistory(1, 0);
  }
  return true;
}

Command CommandHistory::GetCommand(int numIndex){

  commandHistory_t *tmp = NULL;

  if(numIndex < 0){
    if(numIndex == GETPREV){
      if(this->historyCurrent == this->history.next){
        tmp = this->historyCurrent;
      } else {
        tmp = this->historyCurrent->prev;
      }
    } else if(numIndex == GETNEXT){
      if(this->historyCurrent == this->history.prev){
        tmp = this->historyCurrent;
      } else {
        tmp = this->historyCurrent->next;
      }
    } else if(numIndex == GETLAST){
      tmp = this->history.prev;
    } else if(numIndex == GETFIRST){
      tmp = this->history.next;
    } else {
      tmp = this->SearchHistory(numIndex);
    }
  } else {
    tmp = this->SearchHistory(numIndex);
  }
  this->historyCurrent = tmp;

  return tmp->command;
}

Charing CommandHistory::GetHistoryInfo(int numIndex, unsigned int numSize){
  
  return "Nothing";
}

bool CommandHistory::UpdateHistory(Command cmd){

  this->historyCurrent->command = cmd;
  return true;
}

int CommandHistory::RemoveHistory(int numIndex, unsigned int numSize){

  int tmpIndexFirst;
  int tmpIndexLast;
  commandHistory_t *tmpFirst;
  commandHistory_t *tmpLast;
  commandHistory_t *tmpNext;

  if(numIndex < 0){
    tmpIndexFirst = this->historySize + numIndex - numSize;
  } else {
    tmpIndexFirst = numIndex;
  }
  if((tmpIndexFirst > this->historySize) || (tmpIndexFirst == 0)){
    return 0;
  }

  tmpIndexLast = tmpIndexFirst + numSize;
  if(tmpIndexLast > this->historySize){
    tmpIndexLast = this->historySize;
  }

  tmpFirst = this->SearchHistory(tmpIndexFirst);
  tmpLast = this->SearchHistory(tmpIndexLast);
  if((tmpFirst == NULL) || (tmpLast == NULL)) return 0;
  tmpFirst->prev->next = tmpLast->next;
  tmpLast->next->prev = tmpFirst->prev;
  
  while(tmpFirst != tmpLast){
    tmpNext = tmpFirst->next;
    delete tmpFirst;
    tmpFirst = tmpNext;
  } 
  delete tmpLast;
  this->historySize -= (numSize + 1);

  return tmpIndexLast - tmpIndexFirst;
}

