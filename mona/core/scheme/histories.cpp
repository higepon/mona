/*!
    \file   histories.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#ifdef MONA
#include "MonaTerminal.h"

using namespace monash;

void MonaTerminal::initHistories()
{
    histories_.add("(load \"/LIBS/SCHEME/fib.scm\")");
    histories_.add("(call-process \"/APPS/MONAFRMS/BITMAP.EX5 /APPS/MONAFRMS/MONALISA.JPG\")");
    histories_.add("(| \"/APPS/HELLO.EX5\" \"/APPS/UPPER.EX5\" \"/APPS/REVERSE.EX5\")");
    histories_.add("(exec \"/LIBS/SCHEME/fib.scm\")");
    histories_.add("(for-each (lambda (w) (print (mona.gui.window-title w))) (filter #/APPS/ (mona.gui.enum-windows)))");
    histories_.add("(pair? (fs-find \"/APPS\"))");
    histories_.add("(ls \"/SERVERS\"");
    histories_.add("(map mona.fs.name (mona.fs.entries))");
    histories_.add("(map mona.fs.name (filter #/EX/ (mona.fs.entries)))");
    histories_.add("(call-process \"/APPS/HELLO.EX5\")");
    histories_.add("(call-process \"/APPS/WGET.EX5 http://10.0.2.2/hello.html\")");
    histories_.add("(| \"/APPS/WGET.EX5 http://10.0.2.2/hello.html\" \"/APPS/UPPER.EX5\")");
    histories_.add("(start-process \"/APPS/MONAFRMS/BITMAP.EX5 /APPS/MONAFRMS/SAKEBI.JPG 50 50\")");
    histories_.add("(start-process \"/APPS/MONAFRMS/BITMAP.EX5 /APPS/MONAFRMS/MONALISA.JPG 50 300\")");
}
#endif // MONA
