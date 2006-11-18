/*!
  \file   main.cpp
  \brief  1_5

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/11/15 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

class Hoge
{
public:
    void hoge()
    {
        printf("hoge\n");
    }
};

template <class T>
struct OpNewCreator
{
    static T* Create()
    {
        return new T;
    }
};

template <class T>
struct NullCreator
{
    static T* Create()
    {
        return (T*)NULL;
    }
};

template <class CreatePolicy, class S>
class Manager
{
public:
    S* Create() {return policy.Create();}

private:
    CreatePolicy policy;
};

int main(int argc, char *argv[])
{
    // 単体でOpNewCreatorを使ってみる
    OpNewCreator<Hoge> hogeCreateor;
    Hoge* h = hogeCreateor.Create();
    h->hoge();

    // 単体でMallocCreatorを使ってみる
    NullCreator<Hoge> hogeCreateor2;
    Hoge* h2 = hogeCreateor2.Create();
    printf("%x\n", h2);

    // Manager
    Manager<OpNewCreator<Hoge>, Hoge> manager;
    Hoge* h3 = manager.Create();
    h3->hoge();
    return 0;
}
