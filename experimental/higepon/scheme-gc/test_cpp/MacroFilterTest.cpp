#include <string>
#include <fstream>
#include "MacroFilterTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(MacroFilterTest);

using namespace util;
using namespace std;
using namespace monash;

void MacroFilterTest::setUp()
{
}

void MacroFilterTest::tearDown()
{
}

void MacroFilterTest::testFindDefineSyntaxes()
{
    char buf[1024];
    MacroFilter f;
    YAML yaml;
    loadYAML("test_cpp/find_define_syntaxes.yml", yaml);
    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }
    for (int i = 0; i < yaml.size(); i++)
    {
        Strings* s = yaml[i];
        if (s->size() < 2)
        {
            fprintf(stderr, "bad yaml!\n");
        }

        String defineSyntax = s->get(0).data();
        SExps defineSyntaxes;

        // check count of define-syntax
        f.findDefineSyntaxes(SExp::fromString(defineSyntax), defineSyntaxes);
        sprintf(buf, "In %s %d define-syntax found, but should be %d\n", defineSyntax.data(), defineSyntaxes.size(), s->size() - 1);
        CPPUNIT_ASSERT_MESSAGE(buf, defineSyntaxes.size() == (int)(s->size() - 1));

        // check define-syntax exactly same?
        for (int i = 0; i < defineSyntaxes.size(); i++)
        {
            SExp* d        = defineSyntaxes[i];
            SExp* expected = SExp::fromString(s->get(i + 1).data());
            sprintf(buf, "%s unmatch\n %s\n", d->toString().data(), expected->toString().data());
            CPPUNIT_ASSERT_MESSAGE(buf, d->equals(expected));
        }
    }
}

void MacroFilterTest::testFilter()
{
    char buf[1024];
    YAML yaml;
    loadYAML("test_cpp/filter.yml", yaml);
    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }

    for (int i = 0; i < yaml.size(); i++)
    {
        Strings* s = yaml[i];
        if (s->size() != 3)
        {
            fprintf(stderr, "bad yaml!\n");
        }

        String macro     = s->get(0).data();
        String macroCall = s->get(1).data();
        String expected  = s->get(2).data();

        MacroFilter f;
        f.findAndStoreDefineSyntaxes(SExp::fromString(macro));
        SExp* expectedSExp = SExp::fromString(expected);
        SExp* macroCallSExp = SExp::fromString(macroCall);
        f.filter(macroCallSExp);
        sprintf(buf, "[result]\n%s unmatch\n [expected]\n%s\n", macroCallSExp->toSExpString().data(), expectedSExp->toSExpString().data());
        CPPUNIT_ASSERT_MESSAGE(buf, macroCallSExp->equals(expectedSExp));
    }
}
