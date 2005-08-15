/*!
    \file  String.cpp
    \brief  String クラス 実装

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author   
    \version $Revision$
    \date   create: update:$Date$
*/


#include <gui/System/String.h>
#include <gui/System/Pointer.h>

using namespace System;
using namespace System::Text;

namespace System
{
    /*!
    \brief String
         String コンストラクタ
    \author 
    \date   create: update:$Date$
    */
    String::String()
    {
    }

    /*!
    \brief String
         String コンストラクタ char*、サイズ指定
    \param  const char* text [in] 文字列 char*ポインタ
    \param  int length [in] 文字列サイズ          
    \author 
    \date   create: update:$Date$
    */  
    String::String(const char* text, int length /*= -1*/)
    {
        if (text == 0/* NULL */)
        {
            this->Set(_A<wchar>(0));
        }
        else
        {
            this->Set(Encoding::ToWCharArray(text, length));
        }
    }
    

    String::String(const String& text)
    {
        this->Set(text);
    }

    /*!
    \brief String
         String コンストラクタ _A<wchar>指定
    \param  const _A<wchar>& text [in] 文字列 wcharのArray
    \author 
    \date   create: update:$Date$
    */      
    String::String(const _A<wchar>& text)
    {
        this->Set(text);
    }

    /*!
    \brief ~String
         String デスクトラクタ
    \author 
    \date   create: update:$Date$
    */  
    String::~String()
    {
    }

    /*!
    \brief operator ==
        == 関係演算子定義
        pointer メンバと比較する。 全ての文字が一致すればTrue 以外はFalse
    \param  const String& text arg [in] 比較先
    \author 
    \date   create: update:$Date$
    */  
    bool String::operator ==(const String& text)
    {
        int len = this->get_Length();
        if (len != text.get_Length()) return false;
        
        for (int i = 0; i < len; i++)
        {
            if ((*this)[i] != text[i]) return false;
        }
        return true;
    }

    /*!
    \brief operator =
        = 代入演算子定義 Stringクラス
          Setメソッドコール
    \param  const String& text [in] 代入文字列
    \return String&
    \author 
    \date   create: update:$Date$
    */    
    String& String::operator =(const String& text)
    {
        this->Set(text);
        return *this;
    }
    

    /*!
    \brief operator =
        = 代入演算子定義 char* 
          Setメソッドコール
    \param  const char* text [in] 代入文字列
    \return String&
    \author 
    \date   create: update:$Date$
    */
    String& String::operator =(const char* text)
    {
        this->Set(Encoding::ToWCharArray(text, -1));
        return *this;
    }


    /*!
    \brief operator +=
        += 複合代入演算子定義 Stringクラス
         文字列連結として動作
    \param  const String& text [in] 代入文字列
    \return String&
    \author 
    \date   create: update:$Date$
    */      
    void String::operator +=(const String& text)
    {
        int len1 = this->get_Length(), len2 = text.get_Length();
        _A<wchar> s(len1 + len2);
        int p = 0;
        for (int i = 0; i < len1; i++) s[p++] = (*this)[i];
        for (int i = 0; i < len2; i++) s[p++] = text[i];
        this->Set(s);
    }
    

    /*!
    \brief operator +=
        += 複合代入演算子定義 
         文字列連結として動作
    \param  wchar ch [in] 代入文字
    \return String
    \author 
    \date   create: update:$Date$
    */  
    void String::operator +=(wchar ch)
    {
        int len = this->get_Length();
        _A<wchar> s(len + 1);
        int p = 0;
        for (int i = 0; i < len; i++) s[p++] = (*this)[i];
        s[p] = ch;
        this->Set(s);
    }

    /*!
    \brief operator +
        + 算術演算子定義 
         文字列連結として動作
    \param  const String& text [in] 代入文字列
    \return String
    \author 
    \date   create: update:$Date$
    */
    String String::operator +(const String& text)
    {
        String ret = *this;
        ret += text;
        return ret;
    }
    
    /*!
    \brief StartsWith 
        先頭が、指定した String と一致するかどうかを判断
    \param  const String& text [in] 代入文字列
    \return bool
    \author 
    \date   create: update:$Date$
    */
    bool String::StartsWith(const String& value)
    {
        int len = value.get_Length();
        if (len > this->get_Length()) return false;
        for (int i = 0; i < len; i++)
        {
            if ((*this)[i] != value[i]) return false;
        }
        return true;
    }

    /*!
    \brief EndsWith 
        末尾が、指定した String と一致するかどうかを判断
    \param  const String& text [in] 代入文字列
    \return bool
    \author 
    \date   create: update:$Date$
    */
    bool String::EndsWith(const String& value)
    {
        int len = value.get_Length();
        int pos = this->get_Length() - len;
        if (pos < 0) return false;
        for (int i = 0; i < len; i++)
        {
            if ((*this)[pos + i] != value[i]) return false;
        }
        return true;
    }

    /*!
    \brief Substring 
        指定範囲の文字列を抜き出す
    \param  int start [in] スタート位置
    \param  int length [in] 長さ
    \return String
    \author 
    \date   create: update:$Date$
    */
    String String::Substring(int start, int length)
    {
        int len = this->get_Length() - start;
        if (length > len) length = len;
        
        _A<wchar> s(length);
        int p = 0;
        for (int i = 0; i < length; i++) s[p++] = (*this)[start + i];
        return s;
    }
    
}


/*!
\brief operator +
    + 算術演算子定義 2項
     文字列連結として動作
\param  const char* text1 text [in] 連結Src文字列
\param  const String& text2 [in] 連結Dst文字列
\return String
\author 
\date   create: update:$Date$
*/
String operator +(const char* text1, const String& text2)
{
    String ret = text1;
    ret += text2;
    return ret;
}
