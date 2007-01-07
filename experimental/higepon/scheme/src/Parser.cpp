#include "Parser.h"
#include "Assert.h"
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
        node = new Node(Node::NODES);ASSERT(node);
        node->lineno = token.lineno;
        for (;;)
        {
            Node* child = parse();
            if (NULL == child) return node;
            node->nodes.push_back(child);
        }
    case Token::RIGHT_PAREN:
        return NULL;
    case Token::NUMBER:
        node = new Node(Node::NUMBER);ASSERT(node);
        node->value = token.value;
        node->lineno = token.lineno;
        return node;
    case Token::IDENTIFIER:
        node = new Node(Node::SYMBOL);ASSERT(node);
        node->text = token.text;
        node->lineno = token.lineno;
        return node;
    case Token::QUOTE:
        node = new Node(Node::QUOTE);ASSERT(node);
        node->text = token.text;
        node->lineno = token.lineno;
        return node;
    case Token::STRING:
        node = new Node(Node::STRING);ASSERT(node);
        node->text = token.text;
        node->lineno = token.lineno;
        return node;
    default:
        RAISE_ERROR(token.lineno, "unknown token");
    }
    return NULL;
}
