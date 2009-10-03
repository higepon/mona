/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <baygui.h>

class GClock : public Frame {
private:
	Label *label;
	MonAPI::Date *date;
	char time[128];
	
public:
	GClock(){
		setBounds((800 - 212) / 2, (600 - 50) / 2, 212, 50);
		setTitle("とけい");
		date = new MonAPI::Date();
		refreshDate();
		label = new Label(time, Label::CENTER);
		label->setBounds(0, 4, 200, 16);
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
	
	void processEvent(Event *event) {
		if (event->getType() == Event::TIMER) {
			refreshDate();
			label->setText(time);
			setTimer(1000);
		}
	}
};

int main(int argc, char* argv[]) {
	GClock *clock = new GClock();
	clock->run();
	delete(clock);
	return 0;
}
