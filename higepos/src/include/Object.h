/*!
    \file   Object.h
    \brief  basic class Object

    all classes should be the super class of this class

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/
class Object {

  public:
    virtual char* getName() = 0;
};
