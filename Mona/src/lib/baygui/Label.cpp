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

namespace baygui
{
	Label::Label()
	{
	}
	
	Label::~Label()
	{
	}
	
	void Label::setText(const char* text)
	{
		//if (this->text != NULL) delete[] this->text;
		//this->text = new char[strlen(text) + 1];
		//strcpy(this->text, text);
		this->text = text;
		if (this->buffer == NULL) return;
		this->repaint();
	}
	
	void Label::onPaint(_P<Graphics> g)
	{
		g->setColor(this->getBackground());
		g->fillRect(0, 0, this->getWidth(), this->getHeight());
		g->setColor(this->getForeground());
		g->drawText(this->getText(), 0, 0);
	}
}
