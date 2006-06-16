/*!
    \file  Array.h
    \brief  Array テンプレートクラス 定義ヘッダ

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author
    \version $Revision$
    \date   create: update:$Date$
*/


#ifndef __ARRAY_H__
#define __ARRAY_H__

#if __GNUC_MINOR__ < 4
/*!
  \def _P
  \brief ::System::Array
*/
#define _A ::Array
#else
#define _A Array
#endif

#ifdef DEBUG

#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#define STACK_ARRAY(type, name, length) type __##name[length]; _A<type> name(__##name, length, false)
#else
#define STACK_ARRAY(type, name, length) type name[length]
#endif

/*!
  \def FOREACH
  \param  type [in] タイプ
  \param  iterator [in] イテレータ
  \param  array [in] 対象Arrayクラス
  \brief FOREACHマクロ イテレータ
             END_FOREACH と組み合わせて使う。
*/
#define FOREACH(type, iterator, array) \
    if ((array).get_Length() > 0) { type iterator; \
        for (int __##iterator = 0; \
            __##iterator < (array).get_Length() && (&(iterator = (array)[__##iterator]) || true); __##iterator++)
#ifndef END_FOREACH
#define END_FOREACH }
#endif


/*! \class Array
\brief Array struct
      範囲チェック付き配列｡
      参照カウント方式による、参照(ポインタ)の管理も行なう
*/
template <class T> struct Array
{
protected:
    T* pointer;
    int length;
    int* refCount;

private:

    /*!
    \brief Initialize
         Array 初期化
    \author
    \date   create: update:$Date$
    */
    inline void Initialize()
    {
        this->pointer  = 0 /*NULL*/;
        this->length   = 0;
        this->refCount = 0 /*NULL*/;
    }

public:

    /*!
    \brief Array
         Array コンストラクタ
    \author
    \date   create: update:$Date$
    */
    Array()
    {
        this->Initialize();
    }

    /*!
    \brief Array
         Array コンストラクタ
    \param  int length [in] 配列サイズ
    \author
    \date   create: update:$Date$
    */
    Array(int length)
    {
        this->Initialize();
        this->Alloc(length);
    }

    /*!
    \brief Array
         Array コンストラクタ
    \param  T* pointer [in] ポインタ
    \param  int length [in] 配列サイズ
    \param  bool isManaged [in] マネージ(管理)するかどうか？
    \author
    \date   create: update:$Date$
    */
    Array(T* pointer, int length, bool isManaged = true)
    {
        this->Initialize();
        this->Set(pointer, length, isManaged);
    }

    /*!
    \brief Array
         Array コピーコンストラクタ
    \param  const Array<T>& array [in] コピー元
    \author
    \date   create: update:$Date$
    */
    Array(const Array<T>& array)
    {
        this->Initialize();
        this->Set(array);
    }

    /*!
    \brief ~Array
         Array デスクトラクタ
    \author
    \date   create: update:$Date$
    */
    virtual ~Array()
    {
        this->Unset();
    }

    /*!
    \brief Alloc
        Alloc メソッド メモリ領域確保
    \param  int length [in] 配列サイズ
    \return void
    \author
    \date   create: update:$Date$
    */
    void Alloc(int length)
    {
        this->Unset();
        this->Set(new T[length], length);
    }

    /*!
    \brief Set
        Set メソッド 一般用
    \param  T* pointer [in] 対象ポインタ
    \param  int length [in] 配列サイズ
    \param  bool isManaged [in] マネージ(管理)するかどうか？
    \return void
    \author
    \date   create: update:$Date$
    */
    void Set(T* pointer, int length, bool isManaged = true)
    {
        this->Unset();

        this->pointer = pointer;
        this->length = length;
#ifdef DEBUG
        if (this->pointer == 0 /*NULL*/ && this->length > 0)
        {
            //領域を確保していないのに、lengthが > 0 なら out of memory エラー
            ::printf("ERROR: [Array] out of memory\n");
            ::exit(1);
        }
#endif

         //参照カウンタ初期化
         if (this->pointer != 0 /*NULL*/ && isManaged) this->refCount = new int(1);
     }


     /*!
     \brief Set
         Set メソッド コピー用
     \param  const Array<T>& array [in] コピー元
     \return void
     \author
     \date   create: update:$Date$
     */
     void Set(const Array<T>& array)
     {
         this->Unset();

         this->pointer = array.pointer;
         this->length = array.length;
#ifdef DEBUG
        if (this->pointer == 0 /*NULL*/ && this->length > 0)
        {
            ::printf("ERROR: [Array] out of memory\n");
            ::exit(1);
        }
#endif

        this->refCount  = array.refCount;
        if (this->refCount != 0 /*NULL*/) (*this->refCount)++;
    }

    /*!
    \brief Clear
        Clear メソッド
        Array 強制初期化
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
                delete this->refCount;
                this->refCount = 0 /*NULL*/;
                delete [] this->pointer;
            }
            else
            {
                this->refCount = 0 /*NULL*/;
            }
            this->pointer = 0 /*NULL*/;
            this->length = 0;
        }
    }

    /*!
    \brief get
        対象ポインタゲッター メソッド
    \return T*
    \author
    \date   create: update:$Date$
    */
    inline T* get() { return this->pointer; }

    /*!
    \brief get_Length
        配列サイズゲッター メソッド
    \return int 参照カウント
    \author
    \date   create: update:$Date$
    */
    inline int get_Length() const { return this->length; }

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
    \param  const Array<T>& arg [in] 比較先
    \author
    \date   create: update:$Date$
    */
    inline bool operator ==(const Array<T>& arg) const { return this->pointer == arg.pointer; }

    /*!
    \brief operator !=
        != 関係演算子定義
        pointer メンバと比較する。
    \param  const Array<T>& arg [in] 比較先
    \author
    \date   create: update:$Date$
    */
    inline bool operator !=(const Array<T>& arg) const { return this->pointer != arg.pointer; }


    /*!
    \brief operator =
        = 代入演算子定義
          Setメソッドコール
    \param  Object* pointer [in] 代入
    \return Array<T>&
    \author
    \date   create: update:$Date$
    */
    inline Array<T>& operator =(const Array<T>& pointer)
    {
        this->Set(pointer);
        return *this;
    }

    /*!
    \brief operator []
        [] 配列演算子定義
          範囲チェック付き、配列要素アクセス
    \param  int index [in] 添え字
    \return T&
    \author
    \date   create: update:$Date$
    */
    inline T& operator [](int index)
    {
#ifdef DEBUG
        if (this->pointer == 0 /*NULL*/)
        {
            ::printf("ERROR: [Array] null reference exception\n");
            ::exit(1);
        }
        else if (index < 0 || this->length - 1 < index)
        {
            ::printf("ERROR: [Array] out of range (%d/%d)\n", index, this->length);
            ::exit(1);
        }
#endif
        return this->pointer[index];
    }


    /*!
    \brief GetValue
          範囲チェック付き、配列要素アクセス
    \param  int index [in] 添え字
    \return T
    \author
    \date   create: update:$Date$
    */
    inline T GetValue(int index) const
    {
#ifdef DEBUG
        if (this->pointer == 0 /*NULL*/)
        {
            ::printf("ERROR: [Array] null reference exception\n");
            ::exit(1);
        }
        else if (index < 0 || this->length - 1 < index)
        {
            ::printf("ERROR: [Array] out of range (%d/%d)\n", index, this->length);
            ::exit(1);
        }
#endif
         return this->pointer[index];
     }
};


#endif  // __ARRAY_H__
