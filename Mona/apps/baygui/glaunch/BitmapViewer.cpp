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
#include "BitmapViewer.h"

BitmapViewer::BitmapViewer(char *title, _P<Bitmap> picture)
{
	// 画像の大きさに合わせてウィンドウを大きくする
	this->picture = picture;
	this->setTitle(title);
	this->setRect(0, 22, this->picture->getWidth(), this->picture->getHeight());
}
	
/** 描画ハンドラ */
void BitmapViewer::onPaint(_P<Graphics> g)
{
	g->drawImage(this->picture, 0, 0);
}

#if 0
int MonaMain(List<char*>* pekoe) {
	Application::initialize();
	_P<Bitmap> picture = new Bitmap("/NEKO.JPG");
	if (picture->get() == NULL || picture->getWidth() == 0) {
		printf("%s:%d:ERROR: can not open bitmap!\n", __FILE__, __LINE__);
	} else {
		Application::run(new BitmapViewer("/NEKO.JPG", picture));
		//BitmapViewer *bitmap = new BitmapViewer("/NEKO.JPG", picture);
		//bitmap->run();
	}
	Application::dispose();
	return 0;
}
#endif
