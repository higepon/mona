/*
Copyright (c) 2004 Tino, bayside
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

static dword my_tid = THREAD_UNKNOWN;

static void SleepThread()
{
	MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);
	
	MessageInfo msg;
	for (;;) {
		if (MonAPI::Message::receive(&msg) == 0 && msg.header == MSG_GUI_TIMER) break;
	}
	int interval = msg.arg1;
	bool endless = msg.arg2;
	dword target = msg.arg3;
	
	for (;;) {
		sleep(interval);
		MonAPI::Message::send(my_tid, MSG_GUI_TIMER, target);
		if (endless == false) break;
	}
}

namespace baygui
{
	Timer::Timer() : interval(100), isEndless(true)
	{
		this->sleep_tid = THREAD_UNKNOWN;
	}
	
	Timer::~Timer()
	{
		this->dispose();
	}
	
	void Timer::dispose()
	{
		this->stop();
	}
	
	void Timer::start()
	{
		if (this->sleep_tid != THREAD_UNKNOWN) return;
		if (my_tid == THREAD_UNKNOWN) my_tid = syscall_get_tid();
		
		dword id = syscall_mthread_create((dword)SleepThread);
		syscall_mthread_join(id);
		MessageInfo msg, src;
		src.header = MSG_SERVER_START_OK;
		MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
		this->sleep_tid = msg.from;
		MonAPI::Message::send(this->sleep_tid, MSG_GUI_TIMER, this->interval, this->isEndless, (dword)this);
	}
	
	void Timer::stop()
	{
		if (this->sleep_tid != THREAD_UNKNOWN) {
			syscall_kill_thread(this->sleep_tid);
		}
	}
}
