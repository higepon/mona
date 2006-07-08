#include <string>

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

typedef struct Node
{
    int type;
    int nodetype;
    struct Node* left;
    struct Node* right;
    std::string text;
    int value;
    enum
    {
        SYMBOL,
        BODY,
        NUMBER,
        ARGS,
        FUNCTION_CALL,
        FUNCTION_ARGUMENT,
    };
} Node;

Token toknize();
void printNode(Node* node);
Node* parseRight();
Node* parseLeft();
