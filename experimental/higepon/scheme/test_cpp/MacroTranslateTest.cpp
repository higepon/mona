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
    Translator translator;

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() != 3)
        {
            fprintf(stderr, "bad yaml!\n");
        }

        string macro = s->at(0).c_str();
        string fromCode = s->at(1).c_str();
        Node* from = Node::fromString(fromCode);
        string expected = s->at(2).c_str();

        Node* definSyntax = Node::fromString(macro);
        string name = definSyntax->nodes[1]->text;

        translator.translateDefineSyntax(Node::fromString(macro));

        Node* expanded = NULL;
        Node* tmp = from;

    Tokenizer tokenizer(fromCode);
    Parser parser(&tokenizer);
    Node* node = parser.parse();

    node->print();

    Object* object = NULL;
    translator.reset();
    if (translator.translate(&node, &object) != Translator::SUCCESS)
    {

    }
    expanded = node;//translator.getExpandedNode();

//        for (;;)
//        {
//            tmp = translator.expandMacroIfMatch(name, tmp);
//         Object* o;
//         translator.translate(
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             if (NULL == tmp) break;
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             expanded = tmp;

            //      }
    node->print();
        if (NULL == expanded)
        {
            string msg = "translate ";
            msg += macro + " : " + fromCode;
            CPPUNIT_ASSERT_MESSAGE(msg.c_str(), false);
        }
        else
        {
            string msg = "\n[expected]\n" + Node::fromString(expected)->toString() + "\n[translated]\n" + expanded->toString();
            CPPUNIT_ASSERT_MESSAGE(msg, Node::fromString(expected)->equals(expanded));
        }
    }

}
