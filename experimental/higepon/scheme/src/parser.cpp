#include "parser.h"
#include <stack>

using namespace std;

stack<Token> tokens;

Node* parseLeft()
{
    Token token = toknize();
    Node* node = NULL;
    switch(token.type)
    {
    case Token::LEFT_PAREN:
        token = toknize();
        if (token.type == Token::IDENTIFIER)
        {
            node = new Node();
            node->nodetype = Node::FUNCTION_CALL;
            node->left = parseLeft();
            node->type = Token::IDENTIFIER;
            node->text = token.text;
            if (node->left == NULL)
            {
                node->right = NULL;
            }
            else
            {
                node->right = parseRight();
            }
            return node;
        }
        else
        {
            return parseLeft();
        }

    case Token::RIGHT_PAREN:
        return NULL;
    case Token::NUMBER:
        node = new Node();
        node->nodetype = Node::FUNCTION_ARGUMENT;
        node->left = NULL;
        node->right = NULL;
        node->type = Token::NUMBER;
        node->value = token.value;
        return node;
    case Token::IDENTIFIER:
        node = new Node();
        node->nodetype = Node::FUNCTION_ARGUMENT;
        node->left = NULL;
        node->right = NULL;
        node->type = Node::SYMBOL;
        node->text = token.text;
        return node;
    default:
        printf("error\n");
        exit(1);
    }
}

Node* parseRight()
{
    Token token1 = toknize();
    if (token1.type == Token::RIGHT_PAREN)
    {
        return NULL;
    }
    tokens.push(token1);

    Node* root = NULL;
    root = new Node;
    root->nodetype = Node::FUNCTION_ARGUMENT;
    root->type = Node::ARGS;
    root->left = parseLeft();
    if (root->left != NULL) printf("[%d]", root->left->value);
    if (root->left == NULL)
    {
        root->right = NULL;
    }
    else
    {
        root->right = parseRight();
    }
    return root;

}

const char* nodeType(Node* node)
{
    switch(node->nodetype)
    {
    case Node::FUNCTION_CALL:
        return "FUNCTION_CALL";
    case Node::FUNCTION_ARGUMENT:
        return "FUNCTION_ARGUMENT";
    }
}

void printNode(Node* node)
{
    switch(node->type)
    {
    case Node::SYMBOL:
        printf("%s:SYMBOL[%s]", nodeType(node), node->text.c_str());
        break;
    case Node::ARGS:
        printf("%s:ARGS", nodeType(node));
        break;
    case Node::NUMBER:
        printf("%s:NUMBER[%d]", nodeType(node), node->value);
        break;
    }
}

Token toknize()
{
    int c;
    Token token;

    if (!tokens.empty())
    {
        token = tokens.top();
        tokens.pop();
        return token;
    }

once_more:
    c = getc(stdin);
    switch(c)
    {
    case '(':
        token.type = Token::LEFT_PAREN;
        return token;
    case ')':
        token.type = Token::RIGHT_PAREN;
        return token;
    case '\'':
        token.type = Token::QUOTE;
        return token;
    }
    if (isspace(c)) goto once_more;
    if (isdigit(c))
    {
        int n = 0;
        do {
            n = n * 10 + c - '0';
            c = getc(stdin);
        } while (isdigit(c));
        ungetc(c, stdin);
        token.type = Token::NUMBER;
        token.value = n;
        return token;
    }
    if (c == '\"')
    {
        std::string str("");
        for (;;)
        {
            c = getc(stdin);
            if (c == '\"') break;
            str += c;
        }
        token.text = str;
        token.type = Token::STRING;
        return token;
    }
    else// (isalpha(c))
    {
        std::string str("");
        str += c;
        for (;;)
        {
            c = getc(stdin);
            if (isspace(c) || c == '(' || c == ')' || c== '\'')
            {
                ungetc(c, stdin);
                break;
            }
            str += c;
        }
        token.type = Token::IDENTIFIER;
        token.text = str;
        return token;
    }
    printf("hoge");
    exit(1);
}
