/*
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
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN FOCUS OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if !defined(_FOCUSLISTENER_H_INCLUDED_)
#define _FOCUSLISTENER_H_INCLUDED_

namespace monagui {
	/**
	 �t�H�[�J�X���X�i�[�N���X
	*/
	class FocusListener {
	public:
		/** �t�H�[�J�X�𓾂����ɌĂ΂�� */
		virtual void focusGained(FocusEvent* e) = 0;
		/** �t�H�[�J�X���������Ƃ��ɌĂ΂�� */
		virtual void focusLost(FocusEvent* e) = 0;
	};
}

#endif // _FOCUSLISTENER_H_INCLUDED_