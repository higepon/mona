#include "Node.h"

using namespace monash;
using namespace std;

Node* Node::clone() const
{
    Node* node = new Node(type);
    node->lineno = lineno;
    if (isNodes())
    {
        for (Nodes::const_iterator p = nodes.begin(); p != nodes.end(); ++p)
        {
            node->nodes.push_back((*p)->clone());
        }
    }
    else if(isNumber())
    {
        node->value = value;
    }
    else
    {
        node->text = text;
    }
    return node;
}

string Node::typeToString()
{
    char buffer[256];

    switch(type)
    {
    case NUMBER:
        sprintf(buffer, "NUMBER[%d]\n", value);
        break;
    case SYMBOL:
        sprintf(buffer, "SYMBOL[%s]\n", text.c_str());
        break;
    case STRING:
        sprintf(buffer, "STRING[\"%s\"]\n", text.c_str());
        break;
    case QUOTE:
        sprintf(buffer, "QUOTE[\'%s]\n", text.c_str());
        break;
    case NODES:
        sprintf(buffer, "NODES\n");
        break;
    }
    return string(buffer);
}
#if 0
void Node::extractBindings(Node* m, Node* n, BindMap& bindMap)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (m->isSymbol())
    {
        BindObject b;
        b.node = n;
        bindMap[m->text] = b;
        return;
    }
    else if (m->isNodes() && n->isNodes())
    {
        Nodes::size_type nLength = n->nodes.size();
        Nodes::size_type mLength = m->nodes.size();
        for (Nodes::size_type i = 0; i < m->nodes.size(); ++i)
        {
            if (i == nLength) return;
            Node* mm = m->nodes[i];
            Node* nn = n->nodes[i];
            if (mLength != nLength && mm->isMatchAllKeyword())
            {
                BindObject b;
                for (Nodes::size_type j = i; j < nLength; ++j)
                {
                    b.nodes.push_back(n->nodes[j]);
                }
                bindMap[mm->text] = b;
                return;
            }
            else
            {
               extractBindings(mm, nn, bindMap);
           }
        }
        return;
    }
    else
    {
        RAISE_ERROR(m->lineno, "macro exception \n%s\n%s", m->toString().c_str(), n->toString().c_str());
        return;
    }
}


// macro match しているのが前提
#else
void Node::extractBindings(Node* m, Node* n, BindMap& bindMap)
{
    if (m->isSymbol())
    {
        BindObject b;
        b.node = n;
        bindMap[m->text] = b;
        return;
    }
    else if (m->isNodes() && n->isNodes())
    {
        for (Nodes::size_type i = 0; i < m->nodes.size(); ++i)
        {
            extractBindingsInternal(m->nodes[i], n, i, bindMap);
        }
    }
    else
    {
        exit(-1);
    }
}

void Node::Node::extractBindingsInternal(Node* m, Node* n, Nodes::size_type i, BindMap& bindMap)
{
    if (m->isSymbol())
    {
        BindObject b;
        if (m->isMatchAllKeyword())
        {
            for (Nodes::size_type j = i; j < n->nodes.size(); ++j)
            {
                b.nodes.push_back(n->nodes[j]);
            }
        }
        else
        {
            b.node = n->nodes[i];
        }
        bindMap[m->text] = b;
        return;
    }
    else if (m->isNodes() && n->nodes[i]->isNodes())
    {
        for (Nodes::size_type j = 0; j < m->nodes.size(); ++j)
        {
            extractBindingsInternal(m->nodes[j], n->nodes[i], j, bindMap);
        }
    }
    else
    {
        RAISE_ERROR(m->lineno, "macro exception \n%s\n%s", m->toString().c_str(), n->toString().c_str());
    }
}
#endif
void Node::print(int depth /* = 0 */)
{
    printf(toString().c_str());
}

void Node::toStringInternal(uint32_t depth, string& s)
{
    for (uint32_t i = 0; i < depth; i++)
    {
        s += " ";
    }
    s += typeToString();
    depth++;
    for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        (*it)->toStringInternal(depth, s);
    }
}

string Node::toString()
{
    string ret;
    toStringInternal(0, ret);
    return ret;
}

bool Node::equals(Node* node)
{
    return equalsInternal(this, node);
}

bool Node::equalsInternal(Node* m, Node* n)
{
    if (m->type != n->type) return false;
    if (m->isNodes())
    {
        if (m->nodes.size() != n->nodes.size()) return false;
        for (Nodes::size_type i = 0; i < m->nodes.size(); i++)
        {
            if (!equalsInternal(m->nodes[i], n->nodes[i])) return false;
        }
        return true;
    }

    switch(m->type)
    {
    case NUMBER:
        return m->value == n->value;
    case SYMBOL:
    case STRING:
    case QUOTE:
        return m->text == n->text;
    }
    return false;
}

Node* Node::fromString(const std::string& text)
{
    Tokenizer tokenizer(text);
    Parser parser(&tokenizer);
    return parser.parse();
}

int Node::foreachNode(Node* root, bool (Node::*match)() const, int (Node::*func)(Node* root, Node* node))
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

int Node::foreachNodes(Node* root, int (Node::*f)(Node* root, Node* node))
{
    return foreachNode(root, &Node::isNodes, f);
}

int Node::execLoadSyntax(Node* root, Node* node)
{
    if (node->nodes.size() == 0) return 0;
    Node* left = node->nodes[0];
    if (!left->isSymbol()
        || left->text != "load"
        || node->nodes.size() != 2
        || node->nodes[1]->type != Node::STRING) return 0;

    string path  = node->nodes[1]->text;
    string input = load(path.c_str());
    if (input == "") return 0;
    input = "(begin " + input + ")";
    Nodes::size_type i;
    for (i = 0; i < root->nodes.size(); ++i)
    {
        if (root->nodes[i] == node) break;
    }
    Node* n = Node::fromString(input);
    root->nodes[i] = n;
    return 1;
}

int Node::execLoadSyntaxes()
{
    while (foreachNodes(this, &Node::execLoadSyntax));
    return 0;
}
