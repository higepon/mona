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

    bool isNodes() const { return type == NODES; }
    bool isNumber() const { return type == NUMBER; }
    bool isSymbol() const { return type == SYMBOL; }
    bool isString() const { return type == STRING; }
    bool isQuote() const { return type == QUOTE; }

    Nodes nodes;
    std::string text;
    int value;
    void print(int depth = 0);
    std::string typeToStringDetail();
    bool equals(Node* node);
    bool equalsInternal(Node* m, Node* n);
//    std::string typeToString();
//    std::string toSExpInternal(Node* node);
//    std::string toSExp();
    int type;
    enum
    {
        NODES,
        NUMBER,
        SYMBOL,
        STRING,
        QUOTE,
    };
};

}; // namespace monash

#endif // __NODE_H__
