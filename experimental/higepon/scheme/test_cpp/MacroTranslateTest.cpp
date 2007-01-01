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

        string macro = s->at(0).c_str();
        string code  = s->at(1).c_str();
        string expected = s->at(2).c_str();

        Node* to;
        Translator translator;
        int ret = translator.translateMacro(Node::fromString(macro), Node::fromString(code), &to);
        if (ret != Translator::SUCCESS)
        {
            string msg = "translate ";
            msg += macro + " : " + code;
            CPPUNIT_ASSERT_MESSAGE(msg.c_str(), false);
        }
        else
        {
            string msg = "\n[expected]\n" + Node::fromString(expected)->toString() + "\n[translated]\n" + to->toString();
            CPPUNIT_ASSERT_MESSAGE(msg, Node::fromString(expected)->equals(to));
        }
    }

}
