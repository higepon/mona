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

#ifndef __BAYGUI_ARRAYLIST_H__
#define __BAYGUI_ARRAYLIST_H__

#ifdef DEBUG
#include <monapi.h>
#endif

// foreachマクロ
#define FOREACH_AL(type, iterator, arrayList) \
	if ((arrayList)->get_Count() > 0) {type iterator;\
		for (int __##iterator = 0; \
			__##iterator < (arrayList)->get_Count() && (&(iterator = (arrayList)->get_Item(__##iterator)) || true); __##iterator++)
#ifndef END_FOREACH_AL
#define END_FOREACH_AL }
#endif

namespace baygui
{
	/** 汎用リスト (テンプレートクラス) */
	template <class T> class ArrayList : public Object
	{
	protected:
		/** 初期容量 */
		static const int DefaultCapacity = 16;
		/** ポインター */
		T* pointer;
		/** カウンター */
		int count;
		/** 容量 */
		int capacity;
	
	private:
		/** 初期化 */
		inline void Initialize()
		{
			this->pointer  = new T[ArrayList::DefaultCapacity];
			this->count    = 0;
			this->capacity = ArrayList::DefaultCapacity;
		}
	
	public:
		virtual char* className() { return "baygui.ArrayList"; }
		
		ArrayList() {
			this->Initialize();
		}
		
		virtual ~ArrayList() {
			delete [] this->pointer;
		}
		
		/** リストをクリアする */
		void Clear() {
			delete [] this->pointer;
			this->Initialize();
		}
		
		/** 指定された項目を追加する */
		virtual void Add(T item)
		{
			if (this->count == this->capacity)
			{
				this->capacity <<= 1;
				T* ptr = new T[this->capacity];
				for (int i = 0; i < this->count; i++) ptr[i] = this->pointer[i];
				delete [] this->pointer;
				this->pointer = ptr;
			}
			this->pointer[this->count] = item;
			this->count++;
		}
		
		/** 指定された順番の項目を削除する */
		virtual void RemoveAt(int index)
		{
			this->count--;
			for (int i = index; i < this->count; i++)
			{
				this->pointer[i] = this->pointer[i + 1];
			}
			this->pointer[this->count] = *(new T());
		}
		
		/** 指定された項目を削除する */
		virtual void Remove(T item)
		{
			int idx = this->IndexOf(item);
			if (idx < 0) return;
			
			this->RemoveAt(idx);
		}
		
		/** 指定された項目が何番目に格納されているかを得る */
		virtual int IndexOf(T item)
		{
			for (int i = 0; i < this->count; i++)
			{
				if (this->pointer[i] == item) return i;
			}
			return -1;
		}
		
		/** 生ポインターを得る */
		inline T* get() {
			return this->pointer;
		}
		
		/** 参照カウントを得る */
		inline int get_Count() const {
			return this->count;
		}
		
		/** 容量を得る */
		inline int get_Capacity() const {
			return this->capacity;
		}
		
		/** 指定された順番の項目を得る */
		inline T& get_Item(int index)
		{
	#ifdef DEBUG
			if (index < 0 || this->count - 1 < index) {
				printf("ERROR: [ArrayList] out of range (%d/%d)\n", index, this->count);
				exit(1);
			}
	#endif
			return this->pointer[index];
		}
	};
}

#endif  // __BAYGUI_ARRAYLIST_H__
