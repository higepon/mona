/*!
    \file  Pointer.h
    \brief  Pointer テンプレートクラス 定義ヘッダ

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author   
    \version $Revision$
    \date   create: update:$Date$
*/


#ifndef __SYSTEM_POINTER_H__
#define __SYSTEM_POINTER_H__

/*!
  \def _P
  \brief ::System::Pointer の省略用
     C++マネージ拡張の、__gc ポインタ にあたる？
*/
#define _P ::System::Pointer

#ifdef DEBUG
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

/*!
  \def __CHECK_NULL
  \brief NULLチェックマクロ
     NULL は、Type.hで定義
*/
#define __CHECK_NULL if (this->pointer == NULL) { \
    ::printf("ERROR: [Pointer] null reference exception\n"); ::exit(1); }
#else
#define __CHECK_NULL
#endif

#include <gui/System/Object.h>

namespace System
{
    /*! \class Pointer
    \brief Pointer struct
          各インスタンスの参照(ポインタ)の管理を行なう｡
          参照カウント方式
    */
    template <class T> struct Pointer
    {
    private:
        T* pointer;
        int* refCount;
        bool hasRefCount;
    
        /*!
        \brief Initialize
             Pointer 初期化
        \author 
        \date   create: update:$Date$
        */
        inline void Initialize()
        {
            this->pointer  = 0 /*NULL*/;
            this->refCount = 0 /*NULL*/;
            this->hasRefCount = false;
        }
    
    public:
    
        /*!
        \brief Pointer
             Pointer コンストラクタ
        \author 
        \date   create: update:$Date$
        */
        Pointer()
        {
            this->Initialize();
        }
    
        /*!
        \brief Pointer
             Pointer コンストラクタ
        \param  T* pointer [in] 対象ポインタ
        \param  bool isManaged [in] マネージ(管理)するかどうか？         
        \author 
        \date   create: update:$Date$
        */
        Pointer(T* pointer, bool isManaged)
        {
            this->Initialize();
            this->Set(pointer, isManaged);
        }
    
        /*!
        \brief Pointer
             Pointer コピーコンストラクタ
        \param  const Pointer<T>& pointer [in] コピー元
        \author 
        \date   create: update:$Date$
        */
        Pointer(const Pointer<T>& pointer)
        {
            this->Initialize();
            this->Set(pointer);
        }
        
        /*!
        \brief Pointer
             Pointer コンストラクタ Object* 
        \param  Object* pointer [in] 対象ポインタ
        \author 
        \date   create: update:$Date$
        */
        Pointer(Object* pointer)
        {
            this->Initialize();
            this->Set(pointer);
        }
    
        /*!
        \brief ~Pointer
             Pointer デスクトラクタ
        \author 
        \date   create: update:$Date$
        */
        ~Pointer()
        {
            this->Unset();
        }
        
        
        /*!
        \brief Set
            Set メソッド Object および Object継承クラス用
        \param  Object* pointer [in] 
        \return void
        \author 
        \date   create: update:$Date$
        */
        void Set(Object* pointer)
        {
            this->Unset();
            
            this->pointer = (T*)pointer;
            if (this->pointer == 0 /*NULL*/) return;
            
            //参照カウントは、 Object クラスのものを使う(ポインタを保持)
            this->refCount = pointer->get_RefCountPtr();
            (*this->refCount)++;
        }
    
        /*!
        \brief Set
            Set メソッド 一般用
        \param  T* pointer [in] 対象ポインタ
        \param  bool isManaged [in] マネージ(管理)するかどうか？         
        \return void
        \author 
        \date   create: update:$Date$
        */
        void Set(T* pointer, bool isManaged)
        {
            this->Unset();
    
            this->pointer = pointer;
            if (this->pointer != 0 /*NULL*/ && isManaged)
            {
                //ここで、参照カウントを1に
                this->refCount = new int(1);
                this->hasRefCount = true;
            }
        }
    
        /*!
        \brief Set
            Set メソッド コピー用
        \param  const Pointer<T>& pointer [in] コピー元
        \return void
        \author 
        \date   create: update:$Date$
        */
        void Set(const Pointer<T>& pointer)
        {
            this->Unset();
    
            this->pointer  = pointer.pointer;
            this->refCount = pointer.refCount;
            this->hasRefCount = pointer.hasRefCount;
            if (this->refCount != 0 /*NULL*/) (*this->refCount)++;
        }
    
        /*!
        \brief Clear
            Clear メソッド
            Pointer 強制初期化
        \return void
        \author 
        \date   create: update:$Date$
        */
        void Clear()
        {
            this->Initialize();
        }
    
        /*!
        \brief Unset
            Unset メソッド
            参照カウントを減算し、0になったら deleteする。
        \return void
        \author 
        \date   create: update:$Date$
        */
        void Unset()
        {
            if (this->refCount != 0 /*NULL*/)
            {
                (*this->refCount)--;
                if (*this->refCount < 1)
                {
                    //自前のrefCountを使っていれば、ここで解放
                    if (this->hasRefCount) delete this->refCount;
                    this->refCount = 0 /*NULL*/;
                    delete this->pointer;
                }
                else
                {
                    this->refCount = 0 /*NULL*/;
                }
                this->pointer = 0 /*NULL*/;
            }
        }
    
        /*!
        \brief operator ->
            -> ポインタ演算子定義
            NULLチェック付き、ポインタ演算子
        \author 
        \date   create: update:$Date$
        */
        inline T* operator ->() { __CHECK_NULL return this->pointer; }
        
        /*!
        \brief get
            対象ポインタゲッター メソッド
        \return T*
        \author 
        \date   create: update:$Date$
        */        
        inline T* get() { return this->pointer; }
        
        /*!
        \brief get_RefCount
            参照カウントゲッター メソッド
        \return int 参照カウント
        \author 
        \date   create: update:$Date$
        */             
        inline int get_RefCount() const { return this->refCount != 0 /*NULL*/ ? *this->refCount : -1; }

        /*!
        \brief operator ==
            == 関係演算子定義
            pointer メンバと比較する。
        \param  T* arg [in] 比較先
        \author 
        \date   create: update:$Date$
        */    
        inline bool operator ==(T* arg) const { return this->pointer == arg; }

        /*!
        \brief operator !=
            != 関係演算子定義
            pointer メンバと比較する。
        \param  T* arg [in] 比較先
        \author 
        \date   create: update:$Date$
        */
        inline bool operator !=(T* arg) const { return this->pointer != arg; }

        /*!
        \brief operator ==
            == 関係演算子定義
            pointer メンバと比較する。
        \param  const Pointer<T>& arg [in] 比較先
        \author 
        \date   create: update:$Date$
        */
        inline bool operator ==(const Pointer<T>& arg) const { return this->pointer == arg.pointer; }

        /*!
        \brief operator !=
            != 関係演算子定義
            pointer メンバと比較する。
        \param  const Pointer<T>& arg [in] 比較先
        \author 
        \date   create: update:$Date$
        */
        inline bool operator !=(const Pointer<T>& arg) const { return this->pointer != arg.pointer; }
    
        /*!
        \brief operator =
            = 代入演算子定義
              Setメソッドコール
        \param  Object* pointer [in] 代入
        \return Pointer<T>&
        \author 
        \date   create: update:$Date$
        */    
        inline Pointer<T>& operator =(Object* pointer)
        {
            this->Set(pointer);
            return *this;
        }

        /*!
        \brief operator =
            = 代入演算子定義
              Setメソッドコール
        \param  const Pointer<T>& pointer [in] 代入
        \return Pointer<T>&
        \author 
        \date   create: update:$Date$
        */        
        inline Pointer<T>& operator =(const Pointer<T>& pointer)
        {
            this->Set(pointer);
            return *this;
        }
    };
}

#endif  // __SYSTEM_POINTER_H__
