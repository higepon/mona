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

extern dword __gui_server;

namespace baygui
{
	Bitmap::Bitmap(int width, int height)
	{
		int len = width * height;
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_CREATEBITMAP, width, height, COLOR_WHITE) != 0)
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			return;
		}
		if (msg.arg2 == 0) return;
		
		this->_object = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
		if (this->_object == NULL)
		{
			::printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
			return;
		}
		
		this->buffer.Set(_object->Data, len, false);
		this->width  = width;
		this->height = height;
	}
	
	Bitmap::Bitmap(char* fileName)
	{
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_DECODEIMAGE, 0, 0, 0, fileName))
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			return;
		}
		if (msg.arg2 == 0) return;
		
		this->_object = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
		if (this->_object == NULL)
		{
			::printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
			return;
		}
		
		this->width  = this->_object->Width;
		this->height = this->_object->Height;
		int len = this->width * this->height;
		this->buffer.Set(this->_object->Data, len, false);
	}
	
	Bitmap::~Bitmap()
	{
		this->flush();
	}
	
	void Bitmap::flush()
	{
		if (this->_object == NULL) return;
		
		this->buffer.Unset();
		dword handle = this->getHandle();
		MonAPI::MemoryMap::unmap(handle);
		if (MonAPI::Message::send(__gui_server, MSG_GUISERVER_DISPOSEBITMAP, handle) != 0)
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		}
		this->_object = NULL;
	}
	
	void Bitmap::setPixel(int x, int y, unsigned int c)
	{
		if (x < 0 || this->width <= x || y < 0 || this->height <= y) return;
		
		int a = getAlpha(c);
		if (a == 0) return;
		
		int index = x + y * this->width;
		if (a == 0xff) {
			this->buffer[index] = c;
		} else {
			unsigned int cc = this->buffer[index];
			int r = (getRed(cc) * (255 - a) + getRed(c) * a) / 255;
			int g = (getGreen(cc) * (255 - a) + getGreen(c) * a) / 255;
			int b = (getBlue(cc) * (255 - a) + getBlue(c) * a) / 255;
			this->buffer[index] = 0xFF000000 | (r << 16) | (g << 8) | b;
		}
	}
}
