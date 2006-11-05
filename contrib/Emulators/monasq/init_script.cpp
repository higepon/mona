/**
 *	monaSq スクリプト履歴の初期化文字列
 */
/*
 *	Copyright (c) 2006 okayu punch
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without restriction,
 *	including without limitation the rights to use, copy, modify, merge,
 *	publish, distribute, sublicense, and/or sell copies of the Software,
 *	and to permit persons to whom the Software is furnished to do so,
 *	subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *	THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "monasq.h"


// 履歴のデフォルト設定
const SQChar* g_init_script[] = {

"// Closure with Free Variables Sample\n\
\n\
function bind(func, ...) {\n\
	local boundArg = [];\n\
	for (local i=0; i<vargc; ++i) {\n\
		boundArg.append(vargv[i]);\n\
	}\n\
	return function(...) : (func, boundArg) {\n\
		local callArg = [ this ];\n\
		for (local i=0; i<boundArg.len(); ++i)\n\
			callArg.append( boundArg[i] );\n\
		for (local i=0; i<vargc; ++i)\n\
			callArg.append( vargv[i] );\n\
		return func.acall( callArg );\n\
	}\n\
}\n\
\n\
function print3(arg1, arg2, arg3) {\n\
	print( \"arg1 = \" + arg1 + \"\\n\" );\n\
	print( \"arg2 = \" + arg2 + \"\\n\" );\n\
	print( \"arg3 = \" + arg3 + \"\\n\" );\n\
}\n\
\n\
local f1 = bind(print3, 1, 2);\n\
local f2 = bind(print3, \"A\");\n\
f1(3);\n\
f2(\"B\", \"C\");\n"

,

"//Closure Sample\n\
\n\
local closure = function(i, val) {\n\
	return i + \" : \" + val;\n\
}\n\
\n\
function my_print(func, i, val) {\n\
	local str = func(i, val);\n\
	print( str + \"\\n\" );\n\
}\n\
\n\
local ar = [ \"YELLOW\", \"GREEN\", \"BLUE\", \"RED\" ];\n\
for (local i=0; i<ar.len(); ++i) {\n\
	my_print(closure, i, ar[i]);\n\
}\n"

,

"//Class and Meta-method Sample\n\
\n\
class Stack {\n\
	_array = [];\n\
\n\
	function push(val) {\n\
		_array.append(val);\n\
	}\n\
	function pop() {\n\
		return _array.pop();\n\
	}\n\
	function _tostring() {\n\
		local str = \"(Stack : \";\n\
		foreach( i, v in _array ) str += v + \", \";\n\
		return str + \")\";\n\
	}\n\
}\n\
\n\
local stack = Stack();\n\
\n\
stack.push( 128 );\n\
print( stack + \"\\n\" );\n\
\n\
stack.push( \"ABC\" );\n\
print( stack + \"\\n\" );\n\
\n\
stack.push( \"Hello\" );\n\
print( stack + \"\\n\" );\n\
\n\
stack.pop();\n\
print( stack + \"\\n\" );\n"

,

"//Function Sample\n\
\n\
function fact(a) {\n\
	if (a == 0) return 1;\n\
	return a * fact(a - 1);\n\
}\n\
\n\
print(\"fact(1) = \" + fact(1) + \"\\n\");\n\
print(\"fact(10) = \" + fact(10) + \"\\n\");\n"

,

"//--- enter script here. ---\n\
\n\
print(\"Hello, World.\\n\");\n\
\n"

,

"//baygui: Window and Event Handler Sample\n\
\n\
function onMouseDown(x, y, button, src) {\n\
	print(\"mouse down (\" + x + \", \" + y + \")\\n\");\n\
}\n\
\n\
local frame = Frame(\"sample\");\n\
frame.setBounds(10, 10, 250, 200);\n\
frame.setHandler(baygui.MOUSE_PRESSED,\n\
	this, onMouseDown);\n\
frame.run();\n"

,

"//baygui: Drawing String Sample\n\
\n\
class MyFrame extends Frame {\n\
	texts = [\"Hello, MonaOS.\", \"Hello, MonaSq.\"];\n\
	colors = [baygui.blue, baygui.green];\n\
	i = 0;\n\
\n\
	constructor() {\n\
		Frame.constructor(\"Hello\");\n\
		setBounds(10, 10, 120, 120);\n\
		setHandler(baygui.MOUSE_PRESSED,\n\
			this, onMouseDown);\n\
		setHandler(baygui.PAINT,\n\
			this, onPaint);\n\
	}\n\
	function onMouseDown(x, y, button, src) {\n\
		if (i == 0) i = 1;\n\
		else i = 0;\n\
		repaint();\n\
	}\n\
	function onPaint(g) {\n\
		g.setColor(baygui.white);\n\
		g.fillRect(0, 0, 120, 120);\n\
		g.setColor(colors[i]);\n\
		g.drawString(texts[i], 15, 35);\n\
	}\n\
}\n\
\n\
local frame = MyFrame();\n\
frame.run();\n"

,

"//baygui: Drawing Image Sample\n\
\n\
class MyFrame extends Frame {\n\
	image = null;\n\
	constructor(filename) {\n\
		image = Image(filename);\n\
		if (image.getWidth() == 0)\n\
			throw \"file not found\";\n\
		Frame.constructor(\"Image\");\n\
		setBounds(10, 10,\n\
			image.getWidth(), image.getHeight());\n\
		setHandler(baygui.PAINT, this, onPaint);\n\
	}\n\
	function onPaint(g) {\n\
		g.drawImage(image, 0, 0);\n\
	}\n\
}\n\
\n\
local frame = MyFrame(\"/MONAFUN.JPG\");\n\
frame.run();\n"

,

"//baygui: Component Sample\n\
\n\
local frame = Frame(\"sample\");\n\
frame.setBounds(10, 10, 250, 200);\n\
\n\
local textField = TextField();\n\
textField.setBounds(5, 5, 230, 20);\n\
textField.setText(\"Beating the Averages.\");\n\
frame.add(textField);\n\
\n\
local listBox = ListBox();\n\
listBox.setBounds(5, 30, 80, 130);\n\
local list = [\"Frodo\", \"Galadriel\", \"Gandalf\",\n\
	\"Saruman\", \"Wormtongue\", \"Sauron\"];\n\
foreach (v in list) listBox.add(v);\n\
frame.add(listBox);\n\
\n\
local checkbox1 = Checkbox(\"check 1\");\n\
checkbox1.setBounds(90, 60, 140, 20);\n\
frame.add(checkbox1);\n\
local checkbox2 = Checkbox(\"check 2\");\n\
checkbox2.setBounds(90, 80, 160, 20);\n\
frame.add(checkbox2);\n\
local group = CheckboxGroup();\n\
group.add(checkbox1);\n\
group.add(checkbox2);\n\
\n\
local button = Button(\"I'm Feeling Lucky\");\n\
button.setBounds(90, 140, 140, 20);\n\
frame.add(button);\n\
\n\
frame.run();\n"

,

"//monapi: File System Sample\n\
\n\
// list root directory.\n\
local dirInfo = monapi.monapi_call_file_read_directory(\"/\", 0);\n\
foreach(i, v in dirInfo) {\n\
	local str = v.name;\n\
	if (v.attr & monapi.ATTRIBUTE_DIRECTORY) str = str + \"/\";\n\
	print(str+\"\\n\");\n\
}\n\
\n\
// read file \"/BAYGUI.INI\"\n\
local sharedMem = monapi.monapi_call_file_read_data(\"/BAYGUI.INI\", 0);\n\
print( sharedMem.getString(0, -1) );\n\
\n"


};


// 配列の要素数
size_t g_init_script_len = ARRAY_LENGTH(g_init_script);

// 初期表示の履歴位置
int g_default_history_cursor = 4;



// スタートアップスクリプト（起動時に必ず実行する）
const SQChar* g_startup_script =
"\n\
monasq.repeatString <- function(s, cnt) {\n\
	local ret = \"\";\n\
	for (local i=0; i<cnt; ++i) ret += s;\n\
	return ret;\n\
}\n\
\n\
/*\n\
 *	return string that inspects members of the object / table.\n\
 *\n\
 *	object : target object\n\
 *	maxExpand : limit recursive count.\n\
 *	indent, index : internal use.\n\
 */\n\
monasq._getInspectString <- function( object, maxExpand, indent, index ) {\n\
\n\
	local type = typeof object;\n\
	local makeIndentSpace = function(cnt) { return repeatString(\"  \", cnt); }\n\
\n\
	// indent space.\n\
	local space = makeIndentSpace(indent)\n\
	local str = space;\n\
\n\
	// table-index of this object.\n\
	if (index != null) {\n\
		str += index + \" : \";\n\
	}\n\
\n\
	// \n\
	if (type != \"table\" && type != \"array\")\n\
		return str + object.tostring();\n\
\n\
	// digest string of this object.\n\
	str += object.tostring();\n\
\n\
	// show item fold and return, if no more expand.\n\
	if (maxExpand < 0) {\n\
		return str + \" {...}\";\n\
	}\n\
\n\
	// expands members.\n\
	str += \" {\\n\";\n\
	++indent;\n\
\n\
	// iterate members.\n\
	foreach (i, v in object) {\n\
		str += _getInspectString( v, maxExpand - 1, indent, i ) + \"\\n\";\n\
	}\n\
\n\
	return str + space + \"}\";\n\
}\n\
\n\
monasq.getInspectString <- function( value, ... ) {\n\
	local maxExpand = 1;\n\
	if (vargc >= 1) maxExpand = vargv[0];\n\
	return monasq._getInspectString( value, maxExpand, 0, null );\n\
}\n\
\n\
monasq.inspect <- function( value ) {\n\
	print(monasq.getInspectString(value) + \"\\n\");\n\
}\n\
\n\
";

