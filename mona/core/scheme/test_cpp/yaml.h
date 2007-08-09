/*!
    \file  yaml.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#include <scheme.h>
#include <util/String.h>
#include <util/Vector.h>

typedef util::Vector<util::Strings*> YAML;

void loadYAML(const util::String& path, YAML& yaml);

