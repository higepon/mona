#include "MacroFilter.h"

using namespace monash;
using namespace std;

MacroFilter::MacroFilter()
{
}

MacroFilter::~MacroFilter()
{
}

// not used, buf useful for sample
void ForeachNodes(Node* root, void (*f)(Node*))
{
    if (!root->isNodes()) return;
    (*f)(root);
    for (Nodes::iterator p = root->nodes.begin(); p != root->nodes.end(); ++p)
    {
        ForeachNodes(*p, f);
    }
}

int MacroFilter::foreachNodes(Node* root, int (MacroFilter::*f)(Node*))
{
    int ret = 0;
    if (!root->isNodes()) return ret;
    ret  = (this->*f)(root);
    for (Nodes::iterator p = root->nodes.begin(); p != root->nodes.end(); ++p)
    {
        ret += foreachNodes(*p, f);
    }
    return ret;
}

int MacroFilter::tryExpandMacro(Node* root)
{
    if (!root->isNodes() || root->nodes.size() <= 0) return 0;
    Node* left = root->nodes[0];
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (!left->isSymbol()) return 0;

    string name = left->text;
    Macros::iterator p = macros_.find(name);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (p == macros_.end()) return 0;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    Macro* m = (*p).second;

    // todo Macro::Patterを返すべきでは?
    printf("%s:%s\n", name.c_str(), m->name.c_str());
    Node* matchedPattern = m->match(name, root);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (NULL == matchedPattern) return 0;

    BindMap bindMap;
    Node::extractBindings(matchedPattern, root, bindMap);
//    Nodes saved = root->nodes; // copy
//    Node* expanded = macro->patterns[matchedPattern]->clone();
//    expandInternal(expanded, bindMap);
//    return expanded;

    for (BindMap::iterator q = bindMap.begin(); q != bindMap.end(); ++q)
    {
        printf("%s %s:%d %s\n", __func__, __FILE__, __LINE__, q->first.c_str());fflush(stdout);// debug
    }

    Node* expanded = m->patterns[matchedPattern]->clone();
    root->nodes.clear();
    bool isExpanded = false;
    Nodes::size_type nodeSize = expanded->nodes.size();

    // expanded の階層を全て回っておきかえしなくては行けない．
    for (Nodes::size_type i = 0; i < nodeSize; ++i)
    {
        Node* f = expanded->nodes[i];
        printf("%s %s:%d %s\n", __func__, __FILE__, __LINE__,f->text.c_str() );fflush(stdout);// debug
        if (f->isSymbol() && bindMap.find(f->text) != bindMap.end())
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            isExpanded = true;
            if (f->text == "...")
            {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                BindObject b = bindMap[f->text];
                for (Nodes::size_type j = 0; j < b.nodes.size(); j++)
                {
                    root->nodes.push_back(b.nodes[j]);
                }
            }
            else
            {
                printf("%s %s:%d [ %s ] \n", __func__, __FILE__, __LINE__, bindMap[f->text].node->toString().c_str());fflush(stdout);// debug
                root->nodes.push_back(bindMap[f->text].node);
            }
        }
        else
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            root->nodes.push_back(f);
        }
    }
    return isExpanded ? 1 : 0;
}

Node* MacroFilter::filterInternal(Node* node)
{

    return NULL;
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

int MacroFilter::filter(Node* from, Node** to)
{
    while (foreachNodes(from,  &MacroFilter::tryExpandMacro));
    *to = from;
#if 0
    int ret = findAndStoreDefineSyntaxes(from);
    if (SUCCESS != ret) return ret;

    // expand macros
    Node* expanded = expandMacros(from);

    if (expanded)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        *to = expanded;
    }
    else
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        *to = from;
    }
#endif
    return 0;
}

Node* MacroFilter::expandMacros(Node* root)
{
    if (!root->isNodes() || root->nodes.size() <= 0) return NULL;
    Node* left = root->nodes[0];
    if (left->isSymbol())
    {
        printf("%s %s:%d[%s]\n", __func__, __FILE__, __LINE__, left->text.c_str());fflush(stdout);// debug
        Node* ret = expandMacroIfMatch(left->text, root);
        if (ret != NULL)
        {
            Node* r = expandMacros(ret);
            if (r)
            {
                ret = r;
            }
        }
        return ret;
    }
    for (Nodes::size_type i = 0; i < root->nodes.size(); ++i)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        Node* ret =  expandMacros(root->nodes[i]);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        ret->print();
        root->nodes[i] = ret;
    }
    return root;
}

Node* MacroFilter::expandMacroIfMatch(const std::string& name, Node* node)
{
    Macros::iterator p = macros_.find(name);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (p == macros_.end()) return NULL;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    Macro* m = (*p).second;

    // todo Macro::Patterを返すべきでは?
    Node* matchedPattern = m->match(name, node);

    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (NULL == matchedPattern) return NULL;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug

// ここが todo
    return expand(m, matchedPattern, node);
}

Node* MacroFilter::expand(Macro* macro, Node* matchedPattern, Node* node)
{
    BindMap bindMap;
    node->print();
    Node::extractBindings(matchedPattern, node, bindMap);
    Node* expanded = macro->patterns[matchedPattern]->clone();
    expandInternal(expanded, bindMap);
    return expanded;
}

void MacroFilter::expandInternal(Node* from, BindMap& bindMap)
{
    Nodes::size_type nodeSize = from->nodes.size();
    for (Nodes::size_type i = 0; i < nodeSize; ++i)
    {
        Node* f = from->nodes[i];
        if (f->isSymbol() && bindMap.find(f->text) != bindMap.end())
        {
            if (f->text == "...")
            {
                BindObject b = bindMap[f->text];
                for (Nodes::size_type j = 0; j < b.nodes.size(); j++)
                {
                    if (j == 0)
                    {
                        from->nodes[i] = b.nodes[j];
                    }
                    else
                    {
                        from->nodes.insert(from->nodes.begin() + i + j, b.nodes[j]);
                    }
                }
            }
            else
            {
                from->nodes[i] = bindMap[f->text].node;
            }
        }
        else if (f->isNodes())
        {
            expandInternal(f, bindMap);
        }
    }
    return;
}

void MacroFilter::findDefineSyntaxes(Node* root, Nodes& defineSyntaxes)
{
    if (!root->isNodes() || root->nodes.size() <= 0) return;
    Node* left = root->nodes[0];
    if (left->isSymbol() && left->text == "define-syntax")
    {
        defineSyntaxes.push_back(root);
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
