/*!
    \file  Event.h
    \brief  イベント、デリゲート定義ヘッダ

    This file is in the public domain.
    There are no restrictions on any sort of usage of this file.

    \author   
    \version $Revision$
    \date   create: update:$Date$
*/

#ifndef __SYSTEM_EVENT_H__
#define __SYSTEM_EVENT_H__

#include <gui/System/Object.h>
#include <gui/System/Pointer.h>
#include <gui/System/Collections/ArrayList.h>

/*!
  \def DECLARE_DELEGATE
  \param  name [in] デリゲート名
  \param  argtype [in] デリゲート引数タイプ
  \brief デリゲート定義マクロ
     C++マネージ拡張の、__event にあたる。
     下記のクラス、メソッドが定義される。
      \arg I'name' 抽象クラス Invoke メソッドを定義
      \arg 'name' テンプレートクラス 
      \arg 'name'_S クラス
*/
#define DECLARE_DELEGATE(name, argtype) \
    class I##name : public System::Object { public: virtual void Invoke(_P<System::Object>, argtype) = 0; }; \
    template <class T> class name : public I##name { \
    private: \
        _P<T> ptr; void(T::*func)(_P<System::Object>, argtype); \
    public: \
        name(_P<T> p, void(T::*f)(_P<System::Object>, argtype)) : ptr(p), func(f) {} \
        virtual ~name() {} \
        virtual void Invoke(_P<System::Object> sender, argtype arg) { ((ptr.get())->*func)(sender, arg); } \
    }; \
    class name##_S : public I##name { \
    private: \
        void(*func)(_P<System::Object>, argtype); \
    public: \
        name##_S(void(*f)(_P<System::Object>, argtype)) : func(f) {} \
        virtual ~name##_S() {} \
        virtual void Invoke(_P<System::Object> sender, argtype arg) { (*func)(sender, arg); } \
    };


/*!
  \def DECLARE_EVENT
  \param  handler [in] イベントハンドラ
  \param  name [in] イベント名
  \param  argtype [in] イベント 引数タイプ
  \brief イベント定義マクロ
     C++マネージ拡張の、__event にあたる。
     下記のメンバ、メソッドが定義される。
      \arg 'name' privateメンバ 'name' : ハンドラコレクション
      \arg add_'name' publicメソッド ハンドラ追加
      \arg remove_'name' publicメソッド ハンドラ削除
      \arg raise_'name' protectedメソッド 登録済みハンドラコール
*/
#define DECLARE_EVENT(handler, name, argtype) \
    private: \
        _P<System::Collections::ArrayList<_P<handler> > > name; \
    public: \
        void add_##name(_P<handler> delg) { \
            if (this->name == 0 /*NULL*/) this->name = new System::Collections::ArrayList<_P<handler> >; \
            this->name->Add(delg); } \
        void remove_##name(_P<handler> delg) { \
            if (this->name == 0 /*NULL*/) return; \
            this->name->Remove(delg); \
            if (this->name->get_Count() == 0) this->name.Unset(); } \
    protected: \
        void raise_##name(_P<System::Object> sender, argtype e) { \
            if (this->name == 0 /*NULL*/) return; \
            FOREACH_AL(_P<handler>, h, this->name) h->Invoke(sender, e); } END_FOREACH_AL


namespace System
{
    /*! \class EventArgs
    \brief EventArgsクラス
          イベント、デリゲート の引数として用いられる。
    */
    class EventArgs : public Object
    {
    public:
        virtual String get_TypeName() { return "System.EventArgs"; }
        EventArgs() {};
        virtual ~EventArgs() {};
        
        inline static _P<EventArgs> get_Empty() { return new EventArgs(); }
    };
    
    //ここで、DECLARE_DELEGATE 宣言
    //Event.h をinclude するだけで、そのクラスはイベントの各メソッド(add_**,remove_**,raise_**)を備え
    // EventArgs宣言。デリゲートクラス(C++ではデリゲートは言語仕様としては無いので
    // 実際はInvokeメソッドを備える EventHandler テンプレートクラス) を宣言した事と等価となる。
    DECLARE_DELEGATE(EventHandler, _P<EventArgs>)
}

#endif  // __SYSTEM_EVENT_H__
