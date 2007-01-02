#include "Macro.h"

using namespace monash;
using namespace std;

string Macro::error;

Macro::Macro(const string& name) : name(name)
{
}

Macro::~Macro()
{
}

void Macro::addPattern(Node* pattern, Node* definition)
{
    patterns.insert(pair<Node*, Node*>(pattern, definition));
}

bool Macro::checkReservedWord(Node* macro, Node* target, const strings& reservedWords)
{
    if (!isReservedWord(macro, reservedWords)) return true;
    if (!target->isSymbol()) return false;
    return macro->text == target-> text;
}

// todo c++ cook book! FIX ME
bool Macro::isReservedWord(Node* node, const strings& reservedWords)
{
    if (!node->isSymbol()) return false;

    for (strings::const_iterator it = reservedWords.begin(); it != reservedWords.end(); ++it)
    {
        if ((*it) == node->text) return true;
    }
    return false;
}

bool Macro::mustBeMacroName(Node* node)
{
    return node->isSymbol() && node->text == "_";
}

bool Macro::isMacroName(Node* node, const string& macroName)
{
    return node->isSymbol() && node->text == macroName;
}

bool Macro::isMatchAllKeyword(Node* node)
{
    return node->isSymbol() && node->text == "...";
}

Node* Macro::match(const string& macroName, Node* target)
{
    for (Macro::Patterns::const_iterator p = patterns.begin(); p != patterns.end(); ++p)
    {
        if (Macro::match(macroName, reservedWords, (*p).first, target)) return (*p).first;
    }
    return NULL;
}

bool Macro::match(const string& macroName, const strings& reservedWords, Node* macro, Node* target)
{
    if (macro->nodes.size() == 0) return true;

    // Nodes length is same
    if (macro->nodes.size() == target->nodes.size())
    {
        return matchInternal(macroName, reservedWords, macro, target);
    }
    else if (macro->nodes.size() < target->nodes.size())
    {
        Node* last = macro->nodes.at(macro->nodes.size() - 1);

        // Only when "..." comes, length unmatch allowed.
        if (!isMatchAllKeyword(last))
        {
            error = "macro arguments length unmatch1";
            return false;
        }
        return matchInternal(macroName, reservedWords, macro, target);
    }
    else
    {
        error = "macro arguments length unmatch2";
        return false;
    }
    return true;
}

bool Macro::matchInternal(const string& macroName, const strings& reservedWords, Node* macro, Node* target)
{
    for (Nodes::size_type i = 0; i < macro->nodes.size(); i++)
    {
        Node* m = macro->nodes[i];
        Node* t = target->nodes[i];
        if (!checkReservedWord(m, t, reservedWords))
        {
            error = "reserved word " + macro->text + " unmatch";
            return false;
        }

        // Howerver "_" comes, macro name does not come.
        if (mustBeMacroName(m) && !isMacroName(t, macroName))
        {
            error = "macro name unmatch";
            return false;
        }
        if (!match(macroName, reservedWords, m, t)) return false;
    }
    return true;
}
