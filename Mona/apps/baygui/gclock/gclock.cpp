/*
Copyright (c) 2004 bayside
All rights reserved.

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

class GClock : public Window
{
private:
	_P<Label> label1;
	_P<Timer> timer1;
	MonAPI::Date date;

public:
	GClock(){
		label1 = new Label();
		//date = MonAPI::Date();
		
		setLocation((800 - 200 - 12) / 2, (600 - 20 - 28) / 2);
		setClientSize(200, 20);
		setText("とけい");
		
		label1->setRect(4, 4, 196, 16);
		add(label1.get());
		
		timer1 = new Timer();
		timer1->setInterval(500);
		timer1->start();
	}
	
	~GClock(){
		timer1->dispose();
	}
	
	void onEvent(Event *e) {
		if (e->type == 0x40f0) {
			const char* day[] = { "日", "月", "火", "水", "木", "金", "土" };
			const char* ampm[] = { "午前", "午後" };
			char time[128];
			date.refresh();
			sprintf(time, "%d年%02d月%02d日(%s) %s %02d:%02d:%02d",
				date.year(), date.month(), date.day(), day[date.dayofweek() % 7],
				ampm[date.hour() / 12], date.hour() % 12, date.min(), date.sec());
			label1->setText(time);
		}
		Window::onEvent(e);
	}
};

int MonaMain(List<char*>* pekoe) {
	GClock *clock = new GClock();
	clock->run();
	return 0;
}
