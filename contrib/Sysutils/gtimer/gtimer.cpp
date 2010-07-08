/*
Copyright (c) 2005 らっぱ
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <baygui.h>

class GTimer : public Frame {
private:
  TextField *text;
  Button *start;
  int time;

public:
  GTimer(){
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    time = 0;

    setBounds(200,200,120,80);
    setTitle("TIMER");
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    char timeText[128];
    sprintf(timeText,"%d",time);

    start = new Button("START");
    start->setBounds(5,5,100,20);
    add(start);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    text = new TextField();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    text->setBounds(5,30,100,20);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    text->setText(timeText);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    add(text);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  }
  
  ~GTimer(){
    delete(text);
    delete(start);
  }

  void processEvent(Event *event){
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if(event->getType() == MouseEvent::MOUSE_RELEASED){
      if(event->getSource() == start){
	time = atoi(text->getText());
	confirmTime();
	setTimer(1000);
      }
    }else if(event->getType() == Event::TIMER){
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
      time--;
      confirmTime();
      if(time > 0){ setTimer(1000); }
      else{ finish(); }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  }

  void confirmTime(){
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    char timeText[128];
    sprintf(timeText,"%d",time);
    text->setText(timeText);
  }

  void finish(){
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    text->setText("sage");
  }
};

int main(int argc, char* argv[]){

#define MAP_FILE_PATH "/APPS/BAYGUI/GTIMER.MAP"
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        fprintf(stderr, "w3m: stack_trace_enable failed error=%d %d.\n", ret, syscall_get_tid());
    }

  GTimer *timer = new GTimer();
  timer->run();
  delete(timer);
  return 0;
}

