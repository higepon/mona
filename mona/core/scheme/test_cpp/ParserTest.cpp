/*!
    \file   ParserTest.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include <string>
#include <fstream>
#include "scheme.h"
#include "ParserTest.h"
#include "StringReader.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ParserTest);

using namespace std;
using namespace util;
using namespace monash;

void ParserTest::setUp()
{
    scheme_const_init();
}

void ParserTest::tearDown()
{
}

void ParserTest::testParser()
{
    YAML yaml;
    loadYAML("test_cpp/parser.yml", yaml);

    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }

    for (int i = 0; i < yaml.size(); i++)
    {
        Strings* s = yaml[i];
        String extRep = s->get(0)->data();
        StringReader* reader   = new StringReader(extRep);
        Scanner* scanner = new Scanner(reader, reader, NULL);
        Parser parser(scanner);


        Object* o = parser.parse();
        CPPUNIT_ASSERT_MESSAGE(("exptected " + extRep + ", but got " + o->toString()).data(), o->toString() == extRep);
    }
}
