#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include "Tokenizer.h"
#include "Error.h"
#include "Parser.h"

std::string load(const char* file);
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
    BindObject() : node(NULL) {}
public:
    Node* node;
    Nodes nodes;
};


class Node
{
public:
    Node(int type) : type(type), lineno(0) {}
    ~Node() {}

    std::string toString();
    std::string toSExpString();
    void print(int depth = 0);
    bool equals(Node* node);
    Node* clone() const;

    bool isNodes()  const { return type == NODES; }
    bool isNumber() const { return type == NUMBER; }
    bool isSymbol() const { return type == SYMBOL; }
    bool isString() const { return type == STRING; }
    bool isQuote()  const { return type == QUOTE; }
    bool isMatchAllKeyword() const { return isSymbol() && text.find("...") != std::string::npos; }

    static Node* fromString(const std::string& text);
    static void extractBindings(Node* m, Node* n, BindMap& bindMap);

    int foreachNode(Node* root, bool (Node::*match)() const, int (Node::*func)(Node* root, Node* node));
    int foreachNodes(Node* root, int (Node::*f)(Node*, Node*));
#if 0
    int execLoadSyntax(Node* root, Node* node);
    int execLoadSyntaxes();
#endif
    Nodes nodes;
    std::string text;
    int value;
    int type;
    uint32_t lineno;

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
    void toSExpStringInternal(std::string& s);
    std::string typeToString();
    std::string typeToRawString();
};

}; // namespace monash

#endif // __NODE_H__
