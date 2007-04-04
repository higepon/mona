#include <string>
#include <fstream>
#include "Scanner.h"
#include "ScannerTest.h"
#include "StringReader.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ScannerTest);

using namespace std;
using namespace util;
using namespace monash;

void ScannerTest::setUp()
{
}

void ScannerTest::tearDown()
{
}

void ScannerTest::testScaner()
{
    YAML yaml;
    loadYAML("test_cpp/scanner.yml", yaml);

    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }

    for (int i = 0; i < yaml.size(); i++)
    {
        Strings* s = yaml[i];
        Reader* reader = new StringReader(s->get(0)->data());
        Scanner scanner(reader);

        for (int j = 1; j < (s->size() - 1) / 2 + 1; j++)
        {
            SToken* token = scanner.getToken();
            String* type = s->get(j * 2 - 1);
            String* value = s->get(j * 2);
            if (token->typeString() == type->data() && token->valueString() == value->data())
            {
                CPPUNIT_ASSERT_MESSAGE("", true);
            }
            else
            {
                char buf[256];
                sprintf(buf, "test case [%s] expected <%s %s> : but got <%s %s>\n", s->get(0)->data(),type->data(), value->data(), token->typeString().data(), token->valueString().data());
                CPPUNIT_ASSERT_MESSAGE(buf, false);
            }
        }
    }
}
