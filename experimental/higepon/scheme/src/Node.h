#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include "Tokenizer.h"
#include "Parser.h"

namespace monash {

class Node;
typedef std::vector<Node*> Nodes;

#define N(n)         node->nodes[n]
#define NN(i, j)     node->nodes[i]->nodes[j]
#define NNN(i, j, k) node->nodes[i]->nodes[j]->nodes[k]
#define L()          node->nodes.size()
#define LL(n)        node->nodes[n]->nodes.size()


class BindObject;
typedef std::map<std::string, BindObject> BindMap;

class BindObject
{
public:
    Node* node;
    Nodes nodes;
};


class Node
{
public:
    Node(int type) : type(type) {}
    ~Node() {}


    std::string toString();

    void print(int depth = 0);
    bool equals(Node* node);
    Node* clone() const;

    bool isNodes()  const { return type == NODES; }
    bool isNumber() const { return type == NUMBER; }
    bool isSymbol() const { return type == SYMBOL; }
    bool isString() const { return type == STRING; }
    bool isQuote()  const { return type == QUOTE; }

    static Node* fromString(const std::string& text);
    static void extractBindings(Node* m, Node* n, BindMap& bindMap);
    static void extractBindingsInternal(Node* m, Node* n, Nodes::size_type i, BindMap& bindMap);

    Nodes nodes;
    std::string text;
    int value;
    int type;

    enum
    {
        NODES,
        NUMBER,
        SYMBOL,
        STRING,
        QUOTE,
    };

private:
    static bool equalsInternal(Node* m, Node* n);
    void toStringInternal(uint32_t depth, std::string& s);
    std::string typeToString();
};

}; // namespace monash

#endif // __NODE_H__
