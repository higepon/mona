#include "Translator.h"

using namespace monash;

Translator::Translator()
{
}

Translator::~Translator()
{
}

int Translator::translatePrimitive(Node* node, Object** object)
{
    switch(node->type)
    {
    case Node::NUMBER:
        *object = new Number(node->value);
        return SUCCESS;
    case Node::STRING:
        *object = new String(node->text);
        return SUCCESS;
    case Node::QUOTE:
        *object = new Quote(node->text);
        return SUCCESS;
    case Node::SYMBOL:
        *object = new Variable(node->text);
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}

int Translator::translate(Node* node, Object** object)
{
    if (node->type != Node::NODES)
    {
        return translatePrimitive(node, object);
    }

    if (node->nodes.size() <= 0) return SYNTAX_ERROR;

    Node* function = node->nodes[0];
    if (function->type == Node::SYMBOL)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if (function->text == "define")
        {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            if (node->nodes.size() != 3) return SYNTAX_ERROR;
            Node* symbol = node->nodes[1];
            if (symbol->type != Node::SYMBOL) return SYNTAX_ERROR;
            Variable* variable = new Variable(symbol->text);
            Node* argument = node->nodes[2];
            Object* argumentObject;
            if (translate(argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
            *object = new Definition(variable, argumentObject);
            return SUCCESS;
        }
        else if (function->text == "if")
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            if (node->nodes.size() != 4) return SYNTAX_ERROR;
            Object* predicate;
            Object* consequent;
            Object* alternative;
            int ret = translate(node->nodes[1], &predicate);
            if (ret != SUCCESS) return ret;
            ret = translate(node->nodes[2], &consequent);
            if (ret != SUCCESS) return ret;
            ret = translate(node->nodes[3], &alternative);
            if (ret != SUCCESS) return ret;
            *object = new SpecialIf(predicate, consequent, alternative);
            return SUCCESS;
        }
        else if (function->text == "begin")
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            if (node->nodes.size() <= 1) return SYNTAX_ERROR;
            Objects* objects = new Objects;
            for (int i = 1; i < node->nodes.size(); i++)
            {
                Object * object;
                int ret = translate(node->nodes[i], &object);
                if (ret != SUCCESS) return ret;
                objects->push_back(object);
            }
            *object = new Begin(objects);
            return SUCCESS;
        }
        else if (function->text == "lambda")
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            if (node->nodes.size() <= 2) return SYNTAX_ERROR;
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            if (node->nodes[1]->type != Node::NODES) return SYNTAX_ERROR;
            Variables* variables = new Variables;
            for (int i = 0; i < node->nodes[1]->nodes.size(); i++)
            {
                Node* param = node->nodes[1]->nodes[i];
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                if (param->type != Node::SYMBOL) return SYNTAX_ERROR;
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                variables->push_back(new Variable(param->text));
            }

            Objects* body = new Objects;
            for (int i = 2; i < node->nodes.size(); i++)
            {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                Object* o;
                int ret = translate(node->nodes[i], &o);
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                if (ret != SUCCESS) return ret;
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                body->push_back(o);
            }
            *object = new Lambda(body, variables);
            return SUCCESS;
        }
    }
    else
    {
        /* ここを直す 引数なくても余音じゃ？*/
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        Object* f;
        int ret = translate(function, &f);
        Objects* arguments = new Objects;
        for (int i = 1; i < node->nodes.size(); i++)
        {
            Object * object;
            int ret = translate(node->nodes[i], &object);
            if (ret != SUCCESS) return ret;
            arguments->push_back(object);
        }

        *object = new Application(f, arguments);
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}
