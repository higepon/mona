#include "MacroFilter.h"

using namespace monash;
using namespace std;

MacroFilter::MacroFilter()
{
}

MacroFilter::~MacroFilter()
{
}

int MacroFilter::filter(Node* from)
{
    findAndStoreDefineSyntaxes(from);
    Node* root = new Node(Node::NODES);
    Node* left = new Node(Node::SYMBOL);
    left->text = "dummy";
    root->nodes.push_back(left);
    root->nodes.push_back(from);
    tryExpandMacro(NULL, root);
    while (foreachNodes(root,  &MacroFilter::tryExpandMacro));
    return 0;
}

// basic & useful
int MacroFilter::foreachNode(Node* root, bool (Node::*match)() const, int (MacroFilter::*func)(Node* root, Node* node))
{
    int ret = 0;
    if (root->isNodes())
    {
        // don't use iterator here, nodes.size() will be changed by func
        for (Nodes::size_type i = 0; i < root->nodes.size(); i++)
        {
            Node* node = root->nodes[i];
            if ((node->*match)())
            {
                ret += (this->*func)(root, node);
            }
            ret += foreachNode(node, match, func);
        }
    }
    return ret;
}

int MacroFilter::foreachSymbols(Node* root, int (MacroFilter::*f)(Node* root, Node* node))
{
    return foreachNode(root, &Node::isSymbol, f);
}

int MacroFilter::foreachNodes(Node* root, int (MacroFilter::*f)(Node*root, Node* node))
{
    return foreachNode(root, &Node::isNodes, f);
}

int MacroFilter::expandMacro(Node* root, Node* node)
{
    string name = node->text;
    if (bindMap_.find(name) == bindMap_.end()) return 0;

    BindObject b = bindMap_[name];
    Nodes::size_type i;
    for (i = 0; i < root->nodes.size(); ++i)
    {
        if (root->nodes[i] == node) break;
    }
    if (name == "...")
    {
        if (b.nodes.size() == 0)
        {
            root->nodes.erase(root->nodes.begin() + i);
        }
        else
        {
            for (Nodes::size_type j = 0; j < b.nodes.size(); j++)
            {
                if (j == 0)
                {
                    root->nodes[i] = b.nodes[j];
                }
                else
                {
                    root->nodes.insert(root->nodes.begin() + i + j, b.nodes[j]);
                }
            }
        }
    }
    else
    {
        root->nodes[i] = b.node;
    }
    return 1;
}

int MacroFilter::tryExpandMacro(Node* dummy, Node* root)
{
    if (!root->isNodes() || root->nodes.size() <= 0) return 0;
    Node* left = root->nodes[0];
    if (!left->isSymbol()) return 0;

    string name = left->text;
    Macros::iterator p = macros_.find(name);
    if (p == macros_.end()) return 0;
    Macro* m = (*p).second;

    // todo we should return Macro::Pattern?
    Node* matchedPattern = m->match(name, root);
    if (NULL == matchedPattern) return 0;

    BindMap bindMap;
    Node::extractBindings(matchedPattern, root, bindMap);
    Node* expanded = m->patterns[matchedPattern]->clone();
    bindMap_ = bindMap;
    Node* wrap =new Node(Node::NODES);

    // todo fix me! for foreachNode you need to wrap
    wrap->nodes.push_back(expanded);

    int ret = foreachSymbols(wrap, &MacroFilter::expandMacro);
    expanded = wrap->nodes[0];
    if (ret)
    {
        root->nodes.clear();
        for (Nodes::const_iterator p = expanded->nodes.begin(); p != expanded->nodes.end(); ++p)
        {
            root->nodes.push_back(*p);
        }
        root->type = expanded->type;
        root->value = expanded->value;
        root->text = expanded->text;
    }
    return ret;
}

int MacroFilter::findAndStoreDefineSyntaxes(Node* root)
{
    Nodes defineSyntaxes;
    findDefineSyntaxes(root, defineSyntaxes);

    // todo foreach?
    for (Nodes::const_iterator p = defineSyntaxes.begin(); p != defineSyntaxes.end(); ++p)
    {
        int ret = storeDefineSyntaxes(*p);
        if (ret != SUCCESS) return ret;
    }
    return SUCCESS;
}

void MacroFilter::findDefineSyntaxes(Node* root, Nodes& defineSyntaxes)
{
    if (!root->isNodes() || root->nodes.size() <= 0) return;
    Node* left = root->nodes[0];
    if (left->isSymbol() && left->text == "define-syntax")
    {
        defineSyntaxes.push_back(root->clone());

//         // ugly fix me.
//         // define-syntax is replaced to true!
//         root->nodes.clear();
//         root->type = Node::NUMBER;
//         root->value = 1;
        return;
    }

    for (Nodes::const_iterator p = root->nodes.begin() + 1; p != root->nodes.end(); ++p)
    {
        findDefineSyntaxes(*p, defineSyntaxes);
    }
    return;
}

int MacroFilter::storeDefineSyntaxes(Node* node)
{
    if (L() != 3) return SYNTAX_ERROR;
    if (!N(1)->isSymbol()) return SYNTAX_ERROR;
    if (!N(2)->isNodes() || LL(2) < 3) return SYNTAX_ERROR;
    if (!NN(2, 0)->isSymbol() || NN(2, 0)->text != "syntax-rules") return SYNTAX_ERROR;
    if (!NN(2, 1)->isNodes()) return SYNTAX_ERROR;

    // macro name
    Macro* macro = new Macro(N(1)->text);

    // store reserved words
    for (Nodes::const_iterator p = NN(2, 1)->nodes.begin(); p != NN(2, 1)->nodes.end(); ++p)
    {
        Node* n = (*p);
        if (!n->isSymbol()) return SYNTAX_ERROR;
        macro->reservedWords.push_back(n->text);
    }
    // store pattern / definition
    for (Nodes::size_type i = 2; i < LL(2); ++i)
    {
        Node* n = NN(2, i);
        if (!n->isNodes() || n->nodes.size() != 2) return SYNTAX_ERROR;
        macro->addPattern(n->nodes[0], n->nodes[1]);
    }
    macros_[macro->name] = macro;
    return SUCCESS;
}
