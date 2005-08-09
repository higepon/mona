/*
Copyright (c) 2004 Tino
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

#if !defined(_POINTER_H_INCLUDED_)
#define _POINTER_H_INCLUDED_

namespace baygui {
	#define _P baygui::Pointer

	/** 参照カウント型スマートポインター */
	template <class T> struct Pointer {
	private:
		/** 生ポインター */
		T* pointer;
		/** 参照カウント */
		int* refCount;

	private:
		/** 初期化する */
		inline void Initialize()
		{
			this->pointer = 0 /* NULL */;
			this->refCount = 0 /* NULL */;
		}

	public:
		/** デフォルトコンストラクタ */
		Pointer()
		{
			this->Initialize();
		}
		
		/** コピーコンストラクタ */
		Pointer(Object* pointer)
		{
			this->Initialize();
			this->set(pointer);
		}
		
		/** デストラクタ */
		~Pointer()
		{
			this->unset();
		}
		
		/** オブジェクトを設定する */
		void set(Object* pointer)
		{
			this->unset();
			this->pointer = (T*)pointer;
			if (this->pointer == 0 /* NULL */) return;
			this->refCount = pointer->getPointer();
			(*this->refCount)++;
		}
		
		/** オブジェクトを破棄する */
		void unset()
		{
			if (this->refCount != 0 /* NULL */) {
				(*this->refCount)--;
				if (*this->refCount < 1) {
					this->refCount = 0 /*NULL*/;
					delete this->pointer;
				} else {
					this->refCount = 0 /*NULL*/;
				}
				this->pointer = 0 /* NULL */;
			}
		}
		
		/** アロー演算子の多重定義 */
		inline T* operator ->()
		{
			return this->pointer;
		}
		
		/** 生ポインターを得る */
		inline T* get()
		{
			return this->pointer;
		}
		
		/** 参照カウントを得る */
		inline int getRefCount() const
		{
			return this->refCount != 0 ? *this->refCount : -1;
		}
		
		/** ==演算子の多重定義 */
		inline bool operator ==(T* arg) const
		{
			return this->pointer == arg;
		}
		
		/** !=演算子の多重定義 */
		inline bool operator !=(T* arg) const
		{
			return this->pointer != arg;
		}
		
		/** ==演算子の多重定義 */
		inline bool operator ==(const Pointer<T>& arg) const
		{
			return this->pointer == arg.pointer;
		}
		
		/** !=演算子の多重定義 */
		inline bool operator !=(const Pointer<T>& arg) const
		{
			return this->pointer != arg.pointer;
		}
		
		/** 代入演算子の多重定義 */
		inline Pointer<T>& operator =(Object* pointer)
		{
			this->set(pointer);
			return *this;
		}
	};
}

#endif // _POINTER_H_INCLUDED_
