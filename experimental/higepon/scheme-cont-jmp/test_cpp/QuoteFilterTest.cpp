#include <string>
#include <fstream>
#include "QuoteFilterTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(QuoteFilterTest);

using namespace monash;
using namespace std;

void QuoteFilterTest::setUp()
{
}

void QuoteFilterTest::tearDown()
{
}

void QuoteFilterTest::testFilter()
{
    char buf[1024];
    YAML yaml;
    loadYAML("test_cpp/quote_filter.yml", yaml);

    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() != 2)
        {
            fprintf(stderr, "bad yaml!\n");
        }

        string before  = s->at(0).c_str();
        string after   = s->at(1).c_str();
        QuoteFilter f;
        string ret = f.filter(before);
        sprintf(buf, "[result]\n%s unmatch\n [expected]\n%s\n", ret.c_str(), after.c_str());
        CPPUNIT_ASSERT_MESSAGE(buf, ret == after);
    }
}
