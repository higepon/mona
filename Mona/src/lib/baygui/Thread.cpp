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

#include "baygui.h"

/** 静的スレッドID */
static dword staticThreadID = THREAD_UNKNOWN;
/** スレッドインスタンスのアドレス */
static dword threadAddress = 0;

/** コンストラクタ */
Thread::Thread()
{
}

/** デストラクタ */
Thread::~Thread()
{
	syscall_kill_thread(staticThreadID);
}

/** スレッド実行開始 */
void Thread::start()
{
	// 静的スレッド起動
	if (staticThreadID == THREAD_UNKNOWN) {
		staticThreadID = syscall_get_tid();
		MessageInfo info, src;
		dword id = syscall_mthread_create((dword)this->staticFunc);
		syscall_mthread_join(id);
		src.header = MSG_SERVER_START_OK;
		MonAPI::Message::receive(&info, &src, MonAPI::Message::equalsHeader);
		staticThreadID = info.from;
		threadAddress = (dword)this;
	}
}

/** スレッド停止 */
void Thread::stop()
{
	syscall_kill_thread(staticThreadID);
	staticThreadID = THREAD_UNKNOWN;
}

/** スレッドメイン処理 */
void Thread::run()
{
}

/** スレッド実行 */
void Thread::staticFunc()
{
	MonAPI::Message::send(staticThreadID, MSG_SERVER_START_OK);

	while (1) {
		((Thread *)threadAddress)->run();
	}
}
