/*
Copyright (c) 2004 bayside
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

class GClock : public Window {
private:
	Label *label;
	MonAPI::Date *date;
	char time[128];
	
public:
	GClock(){
		setRect((800 - 212) / 2, (600 - 50) / 2, 212, 50);
		setTitle("とけい");
		date = new MonAPI::Date();
		refreshDate();
		label = new Label(time, ALIGN_CENTER);
		label->setRect(0, 4, 200, 16);
		add(label);
		setTimer(1000);
	}
	
	~GClock(){
		delete(label);
		delete(date);
	}
	
	void refreshDate() {
		const char* day[] = { "日", "月", "火", "水", "木", "金", "土" };
		const char* ampm[] = { "午前", "午後" };
		memset(time, 0, sizeof(time));
		date->refresh();
		sprintf(time, "%d年%02d月%02d日(%s) %s %02d:%02d:%02d",
			date->year(), date->month(), date->day(), day[date->dayofweek() % 7],
			ampm[date->hour() / 12], date->hour() % 12, date->min(), date->sec());
	}
	
	void onEvent(Event *event) {
		if (event->getType() == Event::TIMER) {
			refreshDate();
			label->setText(time);
			setTimer(1000);
		}
	}
};

int MonaMain(List<char*>* pekoe) {
	GClock *clock = new GClock();
	clock->run();
	delete(clock);
	return 0;
}
