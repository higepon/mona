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

#ifndef __BAYGUI_POINTER_H__
#define __BAYGUI_POINTER_H__

#define _P baygui::Pointer

#ifdef DEBUG
#include <monapi.h>

#define __CHECK_NULL if (this->pointer == NULL) { \
	printf("ERROR: [Pointer] null reference exception\n"); ::exit(1); }
#else
#define __CHECK_NULL
#endif

namespace baygui
{
	/**
	 スマートポインタ
	*/
	template <class T> struct Pointer
	{
	private:
		/** 生ポインター */
		T* pointer;
		/** 参照カウント */
		int* refCount;
		/** 参照カウントをもつかどうか */
		bool hasRefCount;
	
		inline void Initialize()
		{
			this->pointer  = 0 /*NULL*/;
			this->refCount = 0 /*NULL*/;
			this->hasRefCount = false;
		}
	
	public:
		Pointer() {
			this->Initialize();
		}
		
		Pointer(T* pointer, bool isManaged) {
			this->Initialize();
			this->Set(pointer, isManaged);
		}
		
		Pointer(const Pointer<T>& pointer) {
			this->Initialize();
			this->Set(pointer);
		}
		
		Pointer(Object* pointer) {
			this->Initialize();
			this->Set(pointer);
		}
	
		~Pointer() {
			this->Unset();
		}
		
		void Set(Object* pointer)
		{
			this->Unset();
			
			this->pointer = (T*)pointer;
			if (this->pointer == 0 /*NULL*/) return;
			
			// 参照カウントは Object クラスのものを使う (ポインタを保持)
			this->refCount = pointer->getRefCountPtr();
			(*this->refCount)++;
		}
		
		void Set(T* pointer, bool isManaged)
		{
			this->Unset();
	
			this->pointer = pointer;
			if (this->pointer != 0 /*NULL*/ && isManaged)
			{
				// ここで参照カウントを1にする
				this->refCount = new int(1);
				this->hasRefCount = true;
			}
		}
		
		void Set(const Pointer<T>& pointer)
		{
			this->Unset();
	
			this->pointer  = pointer.pointer;
			this->refCount = pointer.refCount;
			this->hasRefCount = pointer.hasRefCount;
			if (this->refCount != 0 /*NULL*/) (*this->refCount)++;
		}
		
		void Clear() {
			this->Initialize();
		}
		
		void Unset()
		{
			if (this->refCount != 0 /*NULL*/) {
				(*this->refCount)--;
				if (*this->refCount < 1) {
					// 自前の参照カウントを使っていればここで解放する
					if (this->hasRefCount) delete this->refCount;
					this->refCount = 0 /*NULL*/;
					delete this->pointer;
				} else {
					this->refCount = 0 /*NULL*/;
				}
				this->pointer = 0 /*NULL*/;
			}
		}
		
		/** 生ポインターを得る */
		inline T* get() { return this->pointer; }
		
		/** 参照カウントを得る */
		inline int getRefCount() const { return this->refCount != 0 /*NULL*/ ? *this->refCount : -1; }
		
		inline T* operator ->() { __CHECK_NULL return this->pointer; }
		
		inline bool operator ==(T* arg) const { return this->pointer == arg; }
		
		inline bool operator !=(T* arg) const { return this->pointer != arg; }
		
		inline bool operator ==(const Pointer<T>& arg) const { return this->pointer == arg.pointer; }
		
		inline bool operator !=(const Pointer<T>& arg) const { return this->pointer != arg.pointer; }
		
		inline Pointer<T>& operator =(Object* pointer)
		{
			this->Set(pointer);
			return *this;
		}
		
		inline Pointer<T>& operator =(const Pointer<T>& pointer)
		{
			this->Set(pointer);
			return *this;
		}
	};
}

#endif  // __BAYGUI_POINTER_H__
