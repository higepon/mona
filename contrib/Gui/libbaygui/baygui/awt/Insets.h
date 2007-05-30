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
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if !defined(_INSETS_H_INCLUDED_)
#define _INSETS_H_INCLUDED_

namespace baygui {
	/**
	 配置位置クラス
	*/
	class Insets : public Object {
	public:
		/** 左端からの距離 */
		int left;
		/** 上端からの距離 */
		int top;
		/** 下端からの距離 */
		int bottom;
		/** 右端からの距離 */
		int right;
		
	public:
		/** デフォルトコンストラクタ */
		Insets() {
			this->left = this->top = this->bottom = this->right = 0;
		}
		
		/** コンストラクタ */
		Insets(int left, int top, int bottom, int right)
		{
			this->left = left;
			this->top = top;
			this->bottom = bottom;
			this->right = right;
		}
		
		/** デストラクタ */
		virtual ~Insets() {}
	};
}

#endif // _INSETS_H_INCLUDED_
