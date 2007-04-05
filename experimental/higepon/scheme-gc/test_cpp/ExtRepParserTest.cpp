#include <string>
#include <fstream>
#include "scheme.h"
#include "ExtRepParserTest.h"
#include "StringReader.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ExtRepParserTest);

using namespace std;
using namespace util;
using namespace monash;

void ExtRepParserTest::setUp()
{
    scheme_const_init();
}

void ExtRepParserTest::tearDown()
{
}

void ExtRepParserTest::testParser()
{
    YAML yaml;
    loadYAML("test_cpp/ext_rep_parser.yml", yaml);

    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }

    for (int i = 0; i < yaml.size(); i++)
    {
        Strings* s = yaml[i];
        String extRep = s->get(0)->data();
        Reader* reader   = new StringReader(extRep);
        Scanner* scanner = new Scanner(reader);
        ExtRepParser parser(scanner);


        Object* o = parser.parse();
        CPPUNIT_ASSERT_MESSAGE(("exptected " + extRep + ", but got " + o->toString()).data(), o->toString() == extRep);
    }
}
