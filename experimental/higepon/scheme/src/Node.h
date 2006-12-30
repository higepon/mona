#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <string>
#include <stdio.h>

namespace monash {

class Node;
typedef std::vector<Node*> Nodes;

class Node
{
public:
    Node(int type) : type(type) {}
    ~Node() {}

    Nodes nodes;
    int type;
    std::string text;
    int value;
    enum
    {
        NODES,
        NUMBER,
        SYMBOL,
        STRING,
        QUOTE,
    };
    void print(int depth = 0);
    std::string typeToString();
};

}; // namespace monash

#endif // __NODE_H__
