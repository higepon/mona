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

#include "TextViewer.h"

TextViewer::TextViewer(char *path)
{
	setRect(0, 22, 480, 360);
	setTitle(path);
	
	// bzip2 または tek5
	if (path[strlen(path) - 1] == '2' || path[strlen(path) - 1] == '5') {
		monapi_cmemoryinfo* mi = NULL;
		if (path[strlen(path) - 1] == '2') {
			mi = monapi_call_file_decompress_bz2_file(path, false);
		} else {
			mi = monapi_call_file_decompress_st5_file(path, false);
		}
		if (mi != NULL && mi->Size > 0) {
			this->data = (char *)malloc(mi->Size);
			memcpy(this->data, mi->Data, mi->Size);
			monapi_cmemoryinfo_dispose(mi);
			monapi_cmemoryinfo_delete(mi);
		} else {
			this->data = NULL;
		}
	} else {
		dword filesize1 = 0, readsize1 = 0;
		if (syscall_file_open(path, 0, &filesize1) == 0 && filesize1 > 0) {
			this->data = (char *)malloc(filesize1);
			if (syscall_file_read((char*)this->data, filesize1, &readsize1) == 0) {
				syscall_file_close();
			} else {
				free(this->data);
				this->data = NULL;
			}
		} else {
			this->data = NULL;
		}
	}
}

TextViewer::~TextViewer()
{
	free(this->data);
}

/** イベントハンドラ */
void TextViewer::onEvent(Event *e)
{ 
}

/** 描画ハンドラ */
void TextViewer::onPaint(_P<Graphics> g)
{
	g->setColor(COLOR_WHITE);
	g->fillRect(0, 0, this->getWidth(), this->getHeight());
	
	if (this->data != NULL) {
		g->setColor(COLOR_BLACK);
		g->drawText(this->data, 0, 0);
	}
}

#if 0
int MonaMain(List<char*>* pekoe) {
	TextViewer *text = new TextViewer();
	text->run();
	return 0;
}
#endif
