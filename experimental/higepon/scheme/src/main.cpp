/*!
  \file   main.cpp
  \brief  scheme

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/06/16 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "scheme.h"

using namespace monash;
using namespace std;

enum
{
    IDENTIFIER,
    STRING,
    NUMBER,
    LEFT_PAREN,
    RIGHT_PAREN,
    QUOTE,
};

typedef struct Token
{
    int type;
    string text;
    int value;
};

Token toknize()
{
    int c;
    Token token;
once_more:
    c = getc(stdin);
    switch(c)
    {
    case '(':
//        printf("[LEFT_PAREN] ");
        token.type = LEFT_PAREN;
        return token;
    case ')':
//        printf("[RIGHT_PAREN] ");
        token.type = RIGHT_PAREN;
        return token;
    case '\'':
//        printf("[QUOTE] ");
        token.type = QUOTE;
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
//        printf("[NUMBER: %d] ", n);
        token.type = NUMBER;
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
//        printf("[STRING: %s] ", str.c_str());
        token.text = str;
        token.type = STRING;
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
//        ungetc(c, stdin);
        //      printf("[IDENTIFIER: %s] ", str.c_str());
        token.type = IDENTIFIER;
        token.text = str;
        return token;
    }
    printf("hoge");
    exit(1);
}

typedef struct Node
{
    struct Node* left;
    struct Node* right;
    string text;
    int value;
    int op;
} Node;

Node* processLambda()
{

}

enum
{
    OP_SYMBOL,
    OP_NUMBER,
    OP_PARALELL,
};


void printNode(Node* node)
{
    switch(node->op)
    {
    case OP_SYMBOL:
        printf("OP_SYMBOL[%s]", node->text.c_str());
        break;
    case OP_PARALELL:
        printf("OP_PARALELL");
        break;
    case OP_NUMBER:
        printf("OP_SYMBOL[%d]", node->value);
        break;
    }
}

Node* parseRight();
Node* parseLeft();

Node* parseLeft()
{
    Token token = toknize();
    Node* node = NULL;
    switch(token.type)
    {
    case LEFT_PAREN:

        token = toknize();
        if (token.type == IDENTIFIER)
        {
            node = new Node();
            node->left = parseLeft();
            node->op = OP_SYMBOL;
            node->text = token.text;
            if (node->left == NULL)
            {
                node->right = NULL;
            }
            else
            {
                node->right = parseRight();
            }
        }
        return node;
    case RIGHT_PAREN:
        return NULL;
    case NUMBER:
        node = new Node();
        node->left = NULL;
        node->right = NULL;
        node->op = OP_NUMBER;
        node->value = token.value;
        return node;
    case IDENTIFIER:
        node = new Node();
        node->left = NULL;
        node->right = NULL;
        node->op = OP_SYMBOL;
        node->text = token.text;
        return node;
    default:
        printf("error\n");
        exit(1);
    }
}

Node* parseRight()
{
    Token token = toknize();
    if (token.type == RIGHT_PAREN) return NULL;
    Node* root = NULL;
    root = new Node;
    root->op= OP_PARALELL;
    root->left = parseLeft();
    if (root->left == NULL)
    {
        root->right = NULL;
    }
    else
    {
        root->right = parseRight();
    }
    return root;
            
//     switch(token.type)
//     {
//     case LEFT_PAREN:

//         token = toknize();
//         if (token.type == IDENTIFIER)
//         {
//             root = new Node;
//             root->op= OP_PARALELL;
//             root->left = new Node();
//             root->left->left = parseLeft();
//             root->left->op = OP_SYMBOL;
//             root->left->text = token.text;
//             if (root->left->left == NULL)
//             {
//                 root->left->right = NULL;
//             }
//             else
//             {
//                 root->left->right = parseRight();
//             }
//             root->right = parseRight();
//         }
//         return root;
//     case RIGHT_PAREN:
//         return NULL;
//     case NUMBER:
//         root = new Node;
//         root->op = OP_PARALELL;

//         root->left = new Node;
//         root->left->left = NULL;
//         root->left->op= OP_NUMBER;
//         root->left->value = token.value;
//         root->right = parseRight();
//         return root;
//     case IDENTIFIER:
//         root = new Node;
//         root->op = OP_PARALELL;
//         root->left = new Node();
//         root->left->left = NULL;
//         root->left->op= OP_SYMBOL;
//         root->left->text = token.text;
//         return root;
//     default:
//         printf("error\n");
//         exit(1);
//     }
}


// Node* parseParen()
// {
//     Token token = toknize();
//     if (token.type == RIGHT_PAREN)
//     {
//         Node* node = new Node();
//         node->left = parseParen();
//         node->right
//     }
//     else if (token.type == NUMBER)
//     {
//         Node* node = new Node();
//         // hoge hoge
//         return node;
//     }
// }

// Node* parse(Token& token);

// Node* parseIdentifier(Token& token)
// {
//     Node* node = new Node;
//     node->op = OP_SYMBOL;
//     node->text = token.text;
//     Token nextToken = toknize();
//     node->left = parse(nextToken);
//     node->right = NULL;
//     return node;
// }

// Node* parseNumber(Token& token)
// {
//     Node* node = new Node;
//     node->op = OP_NUMBER;
//     node->value = token.value;
//     node->left = NULL;
//     node->right = NULL;
//     return node;
// }

// Node* parse(Token& token)
// {
//     if (token.type == IDENTIFIER)
//     {
//         return parseIdentifier(token);
//     }
//     else if (token.type == NUMBER)
//     {
//         return parseNumber(token);
//     }
//     else if (token.type == LEFT_PAREN)
//     {
//         Token nextToken = toknize();
//         return parse(nextToken);
//     }
//     else
//     {
//         printf("not op code\n");
//         exit(-1);
//     }
//     return NULL;
// }

// Object* createObject()
// {
//     int token = toknize();
//     while (token == LEFT_PAREN)
//     {
//         token = toknize();
//     }

//  = toknize();
//     if (token == LEFT_PAREN)
//     {
//         token = toknize();
//         if (token == Object::DEFINITION)
//         {
//             return new Definition(createObject(), createObject());
//         }
//         return NULL;
//     }


// }


// void hoge()
// {
//     int token = toknize();
//     if (token == LEFT_PAREN)
//     {
//         int t = toknize();
//         switch(c)
//         {
//         case IDENTIFIER:
//             if (str == "define")
//             {


//             }
//         case default:
//             printf("unecpected\n");
//             exit(-1);
//             break;
//         }
//     }
//     else
//     {
//         printf("%s %s:%d exit \n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//         exit(1);
//     }
// }

Object* evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::iterator it = exps->begin(); it != exps->end(); it++)
    {
        ret = eval(*it, env);
    }
    return ret;
}

bool isCompoundProcedure(Object* p)
{
    return p->type() == Object::PROCEDURE;
}

Object* apply(Object* procedure, Objects* arguments)
{
    // fix me
    // if primitive procedure
    if (isCompoundProcedure(procedure))
    {
        Procedure* p = (Procedure*)procedure;
        p->env()->extend(p->parameters(), arguments); // doubt? we need copy?
        evalSequence(p->body(), p->env());
    }

}

bool isTrue(Object* exp)
{
    if (exp->type() == Object::NUMBER)
    {
        Number* n = (Number*)exp;
        if (n->value() == 0) return false;
    }
    return true;
}


void display(Object* exp)
{
    printf("%s", exp->toString().c_str());
}

bool isSelfEvaluating(Object* exp)
{
    if (exp->type() == Object::NUMBER || exp->type() == Object::STRING)
    {
        return true;
    }
    return false;
}

bool is_variable(Object* exp)
{
    return (exp->type() == Object::VARIABLE);
}

bool is_quated(Object* exp)
{
    return (exp->type() == Object::QUOTE);
}

bool is_assignment(Object* exp)
{
    return (exp->type() == Object::ASSIGNMENT);
}

bool is_definition(Object* exp)
{
    return  (exp->type() == Object::DEFINITION);
}

bool isIf(Object* exp)
{
    return  (exp->type() == Object::IF);
}

bool is_lambda(Object* exp)
{
    return (exp->type() == Object::LAMBDA);
}

bool is_begin(Object* exp)
{
    return (exp->type() == Object::BEGIN);
}

bool is_cond(Object* exp)
{
    return false;
}

bool is_application(Object* exp)
{
    return false;
}

Object* text_of_quotation(Object* exp)
{
    return exp; // different from SICP's
}

Object* eval_definition(Object* exp, Environment* env)
{
    Assignment* assignment = (Assignment*)exp;
    env->defineVariable(assignment->variable(), assignment->value());
    return new Quote("OK"); // different from SICP's
}

Object* eval(Object* exp, Environment* env)
{
    if (isSelfEvaluating(exp))
    {
        return exp;
    }
    else if (is_variable(exp))
    {
        return env->lookupVariableValue((Variable*)exp);
    }
    else if (is_quated(exp))
    {
        return text_of_quotation(exp);
    }
    else if (is_assignment(exp))
    {
        Assignment* assignment = (Assignment*)exp;
        return assignment->eval(env);
    }
    else if (is_definition(exp))
    {
        Definition* definition = (Definition*)exp;
        return definition->eval(env);
    }
    else if (isIf(exp))
    {
        return SpecialIf::eval((SpecialIf*)exp, env);
    }
    else if (is_lambda(exp))
    {
        Lambda* lambda = (Lambda*)exp;
        Procedure* procedure = new Procedure(lambda, env);
        return (Object*)procedure;
    }
    else if (is_begin(exp))
    {
        Begin* begin = (Begin*)exp;
        return evalSequence(begin->actions(), env);
    }
    else if (is_cond(exp))
    {
    }
    // quick hack fix me
    else if (exp->type() == Object::PROCEDURE)
    {
        Procedure* procedure = (Procedure*)exp;
        return evalSequence(procedure->body(), procedure->env());
    }
    else if (is_application(exp))
    {
    }
    else
    {
        fprintf(stderr, "Unknown expression type -- EVAL %s ", exp);
    }
    return 0;
}

#include <string>

void printNodes(Node* node, int depth = 0);
void printNodes(Node* node, int depth)
{
    if (NULL == node) return;
    printf("\n");
    for (int i = 0; i < depth; i++) printf(" ");
    printNode(node);
//    printf("[left:");
    depth++;
    printNodes(node->left, depth);
//    printf("]");
//    printf("[right:");
    printNodes(node->right, depth);
//    printf("]");
}

int main(int argc, char *argv[])
{
    printNodes(parseLeft());
    return 0;
    for (;;)
    {
        toknize();

    }
    return 0;
#if 0
    Number a(0);
    Number b(1);
    String c("hoge");
    SpecialIf exp(&a, &b, &c);
    display(eval(&exp, NULL));
#endif
    String str("hige");
    Objects body;
    body.push_back(&str);
    Variables parameters;
    parameters.push_back(new Variable(std::string("val")));
    Lambda lambda(&body, &parameters);
    Environment* env = new Environment();
    Object* o = eval(&lambda, env);
    printf("%s %s:%d [%s]\n", __func__, __FILE__, __LINE__, o->toString().c_str());fflush(stdout);
    Object* c = eval(o, env);
    printf("%s %s:%d [%s]\n", __func__, __FILE__, __LINE__, c->toString().c_str());fflush(stdout);
    //
//    display(eval, env));
    return 0;
}
