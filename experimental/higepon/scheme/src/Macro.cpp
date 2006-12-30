#include "Macro.h"

using namespace monash;

Macro::Macro()
{
}

Macro::~Macro()
{
}

bool Macro::match(const std::string& macroName, Node* macro, Node* target)
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

Node* Macro::toNode(const std::string& text)
{
    Tokenizer tokenizer(text);
    Parser parser(&tokenizer);
    return parser.parse();
}
