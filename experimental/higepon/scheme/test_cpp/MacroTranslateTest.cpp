#include <string>
#include <fstream>
#include "MacroTranslateTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(MacroTranslateTest);

using namespace monash;
using namespace std;

void MacroTranslateTest::setUp()
{
}

void MacroTranslateTest::tearDown()
{
}

void MacroTranslateTest::testFindDefineSyntaxes()
{
    YAML yaml;
    loadYAML("find_define_syntaxes.yml", yaml);
    MacroFilter f;
    char buf[1024];

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() < 2)
        {
            fprintf(stderr, "bad yaml!\n");
        }

        string text = s->at(0);
        Nodes defineSyntaxes;

        // check count of define-syntax
        f.findDefineSyntaxes(Node::fromString(text), defineSyntaxes);
        sprintf(buf, "In %s %d define-syntax found, but should be %d\n", text.c_str(), defineSyntaxes.size(), s->size() - 1);
        CPPUNIT_ASSERT_MESSAGE(buf, defineSyntaxes.size() == s->size() - 1);

        // check define-syntax exactly same?
        for (Nodes::size_type i = 0; i < defineSyntaxes.size(); i++)
        {
            Node* defineSyntax = defineSyntaxes[i];
            Node* expected     = Node::fromString(s->at(i + 1));
            sprintf(buf, "%s unmatch\n %s\n", defineSyntax->toString().c_str(), expected->toString().c_str());
            CPPUNIT_ASSERT_MESSAGE(buf, defineSyntax->equals(expected));
        }
    }
}

void MacroTranslateTest::testTranslate()
{
    YAML yaml;
    // todo rename to filter.yml
    loadYAML("translate.yml", yaml);
    char buf[1024];


    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() != 3)
        {
            fprintf(stderr, "bad yaml!\n");
        }

        string macro     = s->at(0).c_str();
        string macroCall = s->at(1).c_str();
        string expected  = s->at(2).c_str();

        Node* defineSyntax = Node::fromString(macro);
        MacroFilter f;
        f.findAndStoreDefineSyntaxes(defineSyntax);
        Node* expectedNode = Node::fromString(expected);
        Node* macroCallNode = Node::fromString(macroCall);
        f.filter(macroCallNode);
        sprintf(buf, "[result]\n%s unmatch\n [expected]\n%s\n", macroCallNode->toString().c_str(), expectedNode->toString().c_str());
        CPPUNIT_ASSERT_MESSAGE(buf, macroCallNode->equals(expectedNode));
    }


}
