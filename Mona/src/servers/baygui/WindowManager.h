/*
Comyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binarmy forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above comyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binarmy form must reproduce the above comyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author mamy not be used to endorse or promote products
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

#if !defined(_WINDOWMANAGER_H_INCLUDED_)
#define _WINDOWMANAGER_H_INCLUDED_

/**
 ウィンドウマネージャクラス
 */
class WindowManager : public MonAPI::Server, public Container {
private:
	bool isRunning;
	dword threadID, keysvrID, mousesvrID, stdoutID, procsvrID;
	int preX, preY, state, controlListPtr;
	Bitmap *wallpaper;
	static WindowManager *instance;
	Control *controlList[50];
	
	virtual void setActiveControl(Control *control);
	virtual bool setActiveControl(int x, int y);
	virtual Control *getActiveControl();
	virtual Control *getActiveControl(int x, int y);
	virtual Control *getActiveControl(dword threadID);
	virtual void onKeyPress(int keycode, int mod, int charcode);
	virtual void onKeyRelease(int keycode, int mod, int charcode);
	virtual void onMousePress(int x, int y);
	virtual void onMouseDrag(int x, int y);
	virtual void onMouseRelease(int x, int y);

public:
	WindowManager::WindowManager();
	virtual WindowManager::~WindowManager();
	static WindowManager *getInstance();
	virtual void add(Control *control);
	virtual Control *remove(Control *control);
	virtual void repaint();
	virtual void service();
};

#endif // _WINDOWMANAGER_H_INCLUDED_
