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
#include "parser.h"

using namespace monash;
using namespace std;

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

enum {
    SYNTAX_ERROR,
    SUCCESS
};


int createNumberObject(Node* node, Object** object)
{
    *object = new Number(node->value);
    return SUCCESS;
}

int createStringObject(Node* node, Object** object)
{
    *object = new String(node->text);
    return SUCCESS;
}

int createQuoteObject(Node* node, Object** object)
{
    *object = new Quote(node->text);
    return SUCCESS;
}

int createSymbolObject(Node* node, Object** object)
{
    *object = new Variable(node->text);
    return SUCCESS;
}


Environment* environment = new Environment();

int createObject(Node* node, Object** object)
{
    switch(node->type)
    {
    case Node::NUMBER:
        return createNumberObject(node, object);
    case Node::STRING:
        return createStringObject(node, object);
    case Node::QUOTE:
        return createQuoteObject(node, object);
    case Node::SYMBOL:
        return createSymbolObject(node, object);
    }

    if (node->type == Node::NODES && node->nodes.size() > 0)
    {
        //return createApplicationObject(node, object);
        Node* function = node->nodes[0];
        if (function->type  == Node::SYMBOL)
        {
            if (function->text == "define")
            {
                if (node->nodes.size() != 3) return SYNTAX_ERROR;
                Node* symbol = node->nodes[1];
                if (symbol->type != Node::SYMBOL) return SYNTAX_ERROR;
                Variable* variable = new Variable(symbol->text);
                Node* argument = node->nodes[2];
                Object* argumentObject;
                if (createObject(argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
                *object = new Definition(variable, argumentObject);
                return SUCCESS;
            }
            else if (function->text == "if")
            {
                if (node->nodes.size() != 4) return SYNTAX_ERROR;
                Object* predicate;
                Object* consequent;
                Object* alternative;
                int ret = createObject(node->nodes[1], &predicate);
                if (ret != SUCCESS) return ret;
                ret = createObject(node->nodes[2], &consequent);
                if (ret != SUCCESS) return ret;
                ret = createObject(node->nodes[3], &alternative);
                if (ret != SUCCESS) return ret;
                *object = new SpecialIf(predicate, consequent, alternative);
                return SUCCESS;
            }
            else if (function->text == "begin")
            {
                if (node->nodes.size() <= 1) return SYNTAX_ERROR;
                Objects* objects = new Objects;
                for (int i = 1; i < node->nodes.size(); i++)
                {
                    Object * object;
                    int ret = createObject(node->nodes[i], &object);
                    if (ret != SUCCESS) return ret;
                    objects->push_back(object);
                }
                *object = new Begin(objects);
                return SUCCESS;
            }
            else if (function->text == "lambda")
            {

                if (node->nodes.size() <= 2) return SYNTAX_ERROR;

                if (node->nodes[1]->type != Node::NODES) return SYNTAX_ERROR;
                Variables* variables = new Variables;
                for (int i = 0; i < node->nodes[1]->nodes.size(); i++)
                {
                    Node* param = node->nodes[1]->nodes[i];

                    if (param->type != Node::SYMBOL) return SYNTAX_ERROR;

                    variables->push_back(new Variable(param->text));
                }

                Objects* body = new Objects;
                for (int i = 2; i < node->nodes.size(); i++)
                {
                    Object* o;
                    int ret = createObject(node->nodes[i], &o);

                    if (ret != SUCCESS) return ret;

                    body->push_back(o);
                }
                *object = new Lambda(body, variables);
                return SUCCESS;
            }
            else
            {
//                return SUCCESS;


            }
        }
        else if (function->type == Node::NODES)
        {
            Object* f;
            int ret = createObject(function, &f);
            if (ret != SUCCESS) return ret;

            Objects* objects = new Objects;
            for (int i = 1; i < node->nodes.size(); i++)
            {
                Object* o;
                ret = createObject(node->nodes[i], &o);

                if (ret != SUCCESS) return ret;

                objects->push_back(o);
            }

            printf("f->type = %d\n", f->type());
            Object* procedure = eval(f, environment);

            if (procedure->type() != Object::PROCEDURE)
            {

                exit(-1);
            }
            *object = apply(procedure, objects);
            return SUCCESS;
        }
        else
        {
            return SYNTAX_ERROR;
        }
    }
    return SYNTAX_ERROR;
}

bool load(const char* file)
{
    FILE* fp = fopen(file, "rb");
    if (NULL == fp)
    {
        fprintf(stderr, "can not open %s\n", file);
        return false;
    }

    if (-1 == fseek(fp, 0, SEEK_END))
    {
        perror("fseek");
        return false;
    }

    size_t size = ftell(fp);
    char* buffer = new char[size];
    if (NULL == buffer)
    {
        fprintf(stderr, "memory error \n");
        return false;
    }

    fseek(fp, 0, SEEK_SET);

    fread(buffer, 1, size, fp);
    fclose(fp);
    alltext = string(buffer, size);
    delete[] buffer;
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return 0;
    }

    if (!load(argv[1]))
    {
        return -1;
    }

    Node* node = parse();

    node->print();

    Object* object = NULL;
    if (createObject(node, &object) != SUCCESS)
    {
        return 0;
    }

    eval(eval(eval(object, environment), environment), environment);
    printf("environment\n %s", environment->toString().c_str());

//     Environment* environment = new Environment();

//     Node* application = node->nodes[0];
//     if (application->type == Node::NUMBER)
//     {
//         printf("invalid application\n");
//     }
//     else if (application->type == Node::SYMBOL)
//     {
//         if (application->text == "define")
//         {
//             if (node->nodes.size() != 3)
//             {
//                 printf("invalid application\n");
//             }

//             eval(new Definition(new Variable(node->nodes[1]->text), new Number(node->nodes[2]->value)), environment);
//         }
//     }

//     printf("environment\n %s", environment->toString().c_str());


    return 0;
//     for (;;)
//     {
//         toknize();

//     }
//     return 0;
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
