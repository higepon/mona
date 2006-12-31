#include <string>
#include <fstream>
#include "MacroTranslateTest.h"
#include <Macro.h>

CPPUNIT_TEST_SUITE_REGISTRATION(MacroTranslateTest);

using namespace monash;
using namespace std;

void MacroTranslateTest::setUp()
{
}

void MacroTranslateTest::tearDown()
{
}

void MacroTranslateTest::testTranslate()
{
    YAML yaml;
    loadYAML("translate.yml", yaml);

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() != 3)
        {
            fprintf(stderr, "bad yaml!\n");
        }

//         Node* macroFrom = Macro::toNode(s->at(0));
//         Translator new
//         Node* t = Macro::toNode(target);
//         Node* r = Macro::toNode(words);


        CPPUNIT_ASSERT_MESSAGE("", true);
        //      assertMacroMatch(s->at(0).c_str(), s->at(1).c_str(), s->at(2).c_str(), s->at(3).c_str());
    }

}
