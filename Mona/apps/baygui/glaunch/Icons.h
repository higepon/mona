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

#ifndef __LARGEICON_H__
#define __LARGEICON_H__

/** アイコン */
class Icons : public Control
{
private:
	/** アイコンの種類 */
	int type;
	/** ラベル */
	String label;
	/** コマンド */
	String command;
	
public:
	Icons();
	
	virtual ~Icons();
	
	/** アイコンの種類を得る */
	inline int getType()
	{
		return this->type;
	}
	
	/** アイコンの種類を設定する */
	void setType(int type);
	
	/** ラベルを得る */
	inline char* getLabel()
	{
		return this->label.getBytes();
	}
	
	/** ラベルを設定する */
	void setLabel(const char* label);
	
	/** コマンド名を得る */
	inline char* getCommand()
	{
		return this->command.getBytes();
	}
	
	/** コマンド名を設定する */
	void setCommand(const char* command);
	
	/** イベントハンドラ */
	virtual void onEvent(Event *e);
	
	/** 描画ハンドラ */
	virtual void onPaint(_P<Graphics> g);
};

#endif  // __LARGEICON_H__
