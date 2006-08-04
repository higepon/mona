#include "Parser.h"
#include <stdio.h>

using namespace monash;

Parser::Parser(Tokenizer* tokenizer) : tokenizer_(tokenizer)
{
}

Parser::~Parser()
{
}

Node* Parser::parse()
{
    Node* node = NULL;
    Token token = tokenizer_->nextToken();

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
