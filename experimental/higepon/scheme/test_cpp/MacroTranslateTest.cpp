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
        Node* ret;
        Node* expectedNode = Node::fromString(expected);
        f.filter(Node::fromString(macroCall), &ret);
        sprintf(buf, "[result]\n%s unmatch\n [expected]\n%s\n", ret->toString().c_str(), expectedNode->toString().c_str());
        CPPUNIT_ASSERT_MESSAGE(buf, ret->equals(expectedNode));
    }


}
//     YAML yaml;
//     loadYAML("translate.yml", yaml);
//     Translator translator;

//     for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
//     {
//         strings* s = (*it);
//         if (s->size() != 3)
//         {
//             fprintf(stderr, "bad yaml!\n");
//         }

//         string macro     = s->at(0).c_str();
//         string macroCall = s->at(1).c_str();
//         string expected  = s->at(2).c_str();

//         Node* macroNode = Node::fromString(macro);
//         MacroFilter f;
//         f.filter(macroNode, NULL);
//     }


//         Node* from = Node::fromString(fromCode);
//         string expected = s->at(2).c_str();

//         Node* definSyntax = Node::fromString(macro);
//         string name = definSyntax->nodes[1]->text;

//         translator.translateDefineSyntax(Node::fromString(macro));

//         Node* expanded = NULL;
//         Node* tmp = from;

//     Tokenizer tokenizer(fromCode);
//     Parser parser(&tokenizer);
//     Node* node = parser.parse();

//     MacroFilter f;
//     f.filter(node, &node);


//     node->print();

//     Object* object = NULL;
//     translator.reset();
//     if (translator.translate(&node, &object) != Translator::SUCCESS)
//     {

//     }
//     expanded = node;//translator.getExpandedNode();

// //        for (;;)
// //        {
// //            tmp = translator.expandMacroIfMatch(name, tmp);
// //         Object* o;
// //         translator.translate(
// //             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
// //             if (NULL == tmp) break;
// //             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
// //             expanded = tmp;

//             //      }
//     node->print();
//         if (NULL == expanded)
//         {
//             string msg = "translate ";
//             msg += macro + " : " + fromCode;
//             CPPUNIT_ASSERT_MESSAGE(msg.c_str(), false);
//         }
//         else
//         {
//             string msg = "\n[expected]\n" + Node::fromString(expected)->toString() + "\n[translated]\n" + expanded->toString();
//             CPPUNIT_ASSERT_MESSAGE(msg, Node::fromString(expected)->equals(expanded));
//         }
//     }

