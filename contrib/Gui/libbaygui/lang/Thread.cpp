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

#include "baygui.h"

namespace baygui {
	static dword parent_tid = 0;

	/** Monaスレッド */
	static void ExecRun() {
		MonAPI::Message::send(parent_tid, MSG_SERVER_START_OK);
		dword child_tid = syscall_get_tid();
		for (MessageInfo info;;) {
			if (MonAPI::Message::receive(&info) != 0) continue;
			if (info.header == MSG_SERVER_START_OK) {
				Runnable* runnable = (Runnable*)info.arg1;
				runnable->run();
				break;
			}
		}
		syscall_kill_thread(child_tid);
	}

	Thread::Thread(Runnable* runnable)
	{
		parent_tid = MonAPI::System::getThreadID();
		this->runnable = runnable;
	}

	void Thread::start()
	{
		dword thread_id = syscall_mthread_create(ExecRun);
// comment out by higepon
//		syscall_mthread_join(thread_id);
		// MSG_SERVER_START_OKが返ってくるのを待つ
		MessageInfo msg, src;
		src.header = MSG_SERVER_START_OK;
		MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
		dword child_tid = msg.from;
		// Runnable::run() を呼ばせる
		MonAPI::Message::send(child_tid, MSG_SERVER_START_OK, (dword)this->runnable);
	}
}
