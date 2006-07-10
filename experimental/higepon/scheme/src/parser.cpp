#include "parser.h"

using namespace std;

string alltext;
static int current = 0;

int getChar()
{
    if (alltext.size() <= current) return EOF;
    int c = alltext[current];
    current++;
    return c;
}

void unGetChar()
{
    current--;
    if (current < 0) current = 0;
}

Token toknize()
{
    int c;
    Token token;

once_more:
    c = getChar();
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
    if (c == ';')
    {
        for (;;)
        {
            c = getChar();
            if (c == '\n') goto once_more;
        }
    }
    if (isdigit(c))
    {
        int n = 0;
        do {
            n = n * 10 + c - '0';
            c = getChar();
        } while (isdigit(c));
        unGetChar();
        token.type = Token::NUMBER;
        token.value = n;
        return token;
    }
    if (c == '\"')
    {
        std::string str("");
        for (;;)
        {
            c = getChar();
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
            c = getChar();
            if (isspace(c) || c == '(' || c == ')' || c== '\'')
            {
                unGetChar();
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

Node* parse()
{
    Node* node = NULL;
    Token token = toknize();

    switch(token.type)
    {
    case Token::LEFT_PAREN:
        node = new Node(Node::NODES);
        for (;;)
        {
            Node* child = parse();
            if (NULL == child) return node;
            node->nodes.push_back(child);
        }
    case Token::RIGHT_PAREN:
        return NULL;
    case Token::NUMBER:
        node = new Node(Node::NUMBER);
        node->value = token.value;
        return node;
    case Token::IDENTIFIER:
        node = new Node(Node::SYMBOL);
        node->text = token.text;
        return node;
    case Token::QUOTE:
        node = new Node(Node::QUOTE);
        node->text = token.text;
        return node;
    case Token::STRING:
        node = new Node(Node::STRING);
        node->text = token.text;
        return node;
    default:
        printf("unknown token\n");
        exit(-1);
    }
    return NULL;
}
