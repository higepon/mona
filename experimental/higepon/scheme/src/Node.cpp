#include "Node.h"

using namespace monash;
using namespace std;

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

void Node::print(int depth /* = 0 */)
{
    for (int i = 0; i < depth; i++)
    {
        printf(" ");
    }

    printf(typeToString().c_str());

    depth++;
    for (Nodes::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
        (*it)->print(depth);
    }
}
