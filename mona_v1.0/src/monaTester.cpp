/*!
    \file   monaTester.cpp
    \brief  test code

    test code at kernel should be placed here.

    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/11 update:$Date$
*/
#include<monaVga.h>
#include<monaIdt.h>
#include<monaOperator.h>
#include<monaTypes.h>
#include<HVector.h>
#include<monaTester.h>
#include<monaTypes.h>
#include<ProcessManager.h>
#include<KeyBoardManager.h>
#include<Semaphore.h>

#include<string>
#include<list>
#include<vector>
#include<algorithm>
#include<global.h>

/*!
    \brief test code for HVector

    test code for HVector

    \author HigePon
    \date   create:2002/11/11 update:2003/01/26
*/
void hvectorTester() {

    g_console->printf("HVector test start\n");
    HVector<char*>* v = new HVector<char*>(3, 5);
    v->add("one");
    v->add("two");
    v->add("three");
    v->add("four");
    v->add("five");
    v->add("six");
    v->add("seven");
    g_console->printf("element %s removed\n", v->remove(0));
    g_console->printf("element %s removed\n", v->remove(4));
    for (size_t j = 0; j < v->size(); j++) {
        g_console->printf("(%d, %s) ", j, v->get(j));
    }
    g_console->printf("\n");
    delete(v);
}

/*!
    \brief test code for process manager

    test code for process manager

    \author HigePon
    \date   create:2002/11/20 update:$Date$
*/
void processTester() {

    ProcessManager& pm = ProcessManager::instance();
    pm.multiTaskTester();
    return;
}


void process1Tester() {

    while (true) {
        for (dword i = 0; i < 9900000; i++) {
            i += 1;
            i -= 1;
        }
        _sysSetColor(SYS_BG_COLOR | CH_RED);
        g_console->printf("  (.o'v'o)  ");
        _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    }
    return;
}

void process2Tester() {
    enableInterrupt();

    static semaphore sm = 1;

    //    KeyBoardManager& km = KeyBoardManager::instance();
    //    char ch;
    while (true) {

//          if (Semaphore::down(&sm) == 0) {

//              ch = km.getCharacter();
//              if (ch != -1) g_console->printf("%c\n", ch);
//              Semaphore::up(&sm);
//          }

      _sysSetColor(SYS_BG_COLOR | CH_YELLOW);
      g_console->printf("  ( '  v ')  ");
      _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
      for (dword i = 0; i < 99000000; i++) {
          i += 1;
          i -= 1;
      }
    }
    return;
}

void process3Tester() {
    enableInterrupt();

    while (true) {

          _sysSetColor(SYS_BG_COLOR | CH_ORANGE);
          g_console->printf("  ( \"  A \")  ");
          _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
          for (dword i = 0; i < 99000000; i++) {
              i += 1;
              i -= 1;
          }
    }
    return;
}


/*!
    \brief test code for STLport

    test code for STLport

    \author HigePon
    \date   create:2003/01/09 update:2003/01/11
*/
void STLTester() {

    g_console->printf("--------STL Test--------------------------------------------------------\n");

    // string
    std::string str = "---";
    str += "string";
    str += "@mona ";
    str += "is OK";
    str += "---";
    g_console->printf("string: %s\n", str.data());
    g_console->printf("string: str.substr(3, 11): %s\n", str.substr(3, 11).data());

    // list
    std::list<const char*> li;
    li.push_back("micoro kernel ");
    li.push_back("operating system with ");
    li.push_back("network suit architecture ");
    li.push_back("\n");
    std::list<const char*>::iterator it;
    g_console->printf("list  : iterate->");
    for (it = li.begin(); it != li.end(); it++) {
        g_console->printf("%s", *it);
    }

    // vector
    std::vector<const char*> ve;
    ve.push_back("1");
    ve.push_back("2");
    ve.push_back("3");
    ve.push_back("4");
    std::vector<const char*>::iterator it2;
    g_console->printf("vector : iterate->");
    for (it2 = ve.begin(); it2 != ve.end(); it2++) {
        g_console->printf("%s ", *it2);
    }

    // argorithm
    std::reverse(ve.begin(), ve.end());
    g_console->printf("\nargorithm : reverse->");
    for (it2 = ve.begin(); it2 != ve.end(); it2++) {
        g_console->printf("%s ", *it2);
    }

    std::fill(ve.begin(), ve.end(), (char*)"7");
    g_console->printf("\nargorithm : fill with \"7\"->");
    for (it2 = ve.begin(); it2 != ve.end(); it2++) {
        g_console->printf("%s ", *it2);
    }
    g_console->printf("\n--------STL Test END---------------------------------------------------\n");
}
