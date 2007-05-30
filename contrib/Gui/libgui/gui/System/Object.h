/*!
    \file  Object.h
    \brief  Object クラス (基底クラス) 定義ファイル

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author   
    \version $Revision$
    \date   create: update:$Date$
*/


#ifndef __SYSTEM_OBJECT_H__
#define __SYSTEM_OBJECT_H__

#include <gui/System/String.h>

namespace System
{
    /*! \class Object
    \brief Object クラス
          基底クラス
    */  
    class Object
    {
    private:
        int refCount;
    
    public:
        
        /*!
        \brief get_TypeName
             タイプ名取得
        \return String
        \author 
        \date   create: update:$Date$
        */
        virtual String get_TypeName() { return "System.Object"; }
        
        /*!
        \brief ToString
             ToString タイプ名を返す
        \return String             
        \author 
        \date   create: update:$Date$
        */      
        virtual String ToString() { return this->get_TypeName(); }

        /*!
        \brief Object
             Object コンストラクタ
             参照カウントを初期化
        \author 
        \date   create: update:$Date$
        */
        Object() : refCount(0) {}
        
        /*!
        \brief ~Object
             Object デスクトラクタ
        \author 
        \date   create: update:$Date$
        */  
        virtual ~Object() {}

        /*!
        \brief get_RefCount
             参照カウンタゲッター
        \return int
        \author 
        \date   create: update:$Date$
        */
        inline int get_RefCount() { return this->refCount; }

        /*!
        \brief get_RefCountPtr
             参照(へのポインタ)カウンタゲッター
        \return int*
        \author 
        \date   create: update:$Date$
        */
        inline int* get_RefCountPtr() { return &this->refCount; }
    };
}

#endif  // __SYSTEM_OBJECT_H__
