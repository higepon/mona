#include <vector>
#include <string>
#include <stdio.h>

typedef struct Token
{
    int type;
    std::string text;
    int value;
    enum
    {
        IDENTIFIER,
        STRING,
        NUMBER,
        LEFT_PAREN,
        RIGHT_PAREN,
        QUOTE,
    };
};


class Node
{
public:
    Node(int type) : type(type) {}
    ~Node() {}

    typedef std::vector<Node*> Nodes;
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

Node* parse();

extern std::string alltext;
