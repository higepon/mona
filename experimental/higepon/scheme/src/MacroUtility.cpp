#include "MacroUtility.h"

using namespace monash;

MacroUtility::MacroUtility()
{
}

MacroUtility::~MacroUtility()
{
}

bool MacroUtility::match(const std::string macroName, Node* macro, Node* target)
{
    if (macro->nodes.size() != target->nodes.size()) return false;

    for (Nodes::size_type i = 0; i < macro->nodes.size(); i++)
    {
        Node* m = macro->nodes[i];
        Node* t = target->nodes[i];
        if (!match(macroName, m, t)) return false;
    }
    return true;
}

Node* MacroUtility::macroToNode(const std::string text)
{
    Tokenizer tokenizer(text);
    Parser parser(&tokenizer);
    return parser.parse();
}
