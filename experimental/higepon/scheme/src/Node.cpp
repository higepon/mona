#include "Node.h"

using namespace monash;
using namespace std;

Node* Node::clone() const
{
    Node* node = new Node(type);
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

// macro match しているのが前提
void Node::extractBindings(Node* m, Node* n, BindMap& bindMap)
{
    if (m->isSymbol())
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        BindObject b;
        b.node = n;
        bindMap[m->text] = b;
        return;
    }
    else if (m->isNodes() && n->isNodes())
    {
        for (Nodes::size_type i = 0; i < m->nodes.size(); ++i)
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            extractBindingsInternal(m->nodes[i], n, i, bindMap);
        }
    }
    else
    {
        printf("%s", __func__);
        exit(-1);
    }
}

void Node::Node::extractBindingsInternal(Node* m, Node* n, Nodes::size_type i, BindMap& bindMap)
{
    printf("*************************************\n");
    m->print();
    n->print();
    printf("*************************************\n");

    if (m->isSymbol())
    {
        BindObject b;
        printf("%s %s:%d <<%s>>\n", __func__, __FILE__, __LINE__, m->text.c_str());fflush(stdout);// debug
        if (m->text == "...")
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
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        for (Nodes::size_type j = 0; j < m->nodes.size(); ++j)
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            extractBindingsInternal(m->nodes[j], n->nodes[i], j, bindMap);
        }
    }
    else
    {
        printf("*************[%d][%d]", m->isNodes(), n->nodes[i]->type);
        m->print();
        n->print();
        exit(-1);
    }



 }

// string Node::typeToString()
// {
//     char buffer[256];

//     switch(type)
//     {
//     case NUMBER:
//         sprintf(buffer, "%d", value);
//         break;
//     case SYMBOL:
//         sprintf(buffer, "%s", text.c_str());
//         break;
//     case STRING:
//         sprintf(buffer, "\"%s\"", text.c_str());
//         break;
//     case QUOTE:
//         sprintf(buffer, "\'%s", text.c_str());
//         break;
//     }
//     printf("typeToString::%s\n", buffer);fflush(stdout);
//     return string(buffer);
// }

void Node::print(int depth /* = 0 */)
{
    printf(toString().c_str());
//     for (int i = 0; i < depth; i++)
//     {
//         printf(" ");
//     }

//     printf(typeToString().c_str());

//     depth++;
//     for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
//     {
//         (*it)->print(depth);
//     }
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

// string Node::toSExp()
// {
//     return toSExpInternal(this);
// }

// string Node::toSExpInternal(Node* node)
// {
//     string ret;
//     if (node->isNodes())
//     {
//         ret += "(";
//         for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
//         {
//             if (it != nodes.begin()) ret += " ";
//             ret += toSExpInternal(*it);
//         }
//         ret += ")";
//     }
//     else
//     {
//         ret += node->typeToString();
//     }
//     return ret;
// }


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
