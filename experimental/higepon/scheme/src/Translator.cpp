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
        *object = new Number(node->value);ASSERT(*object);
        return SUCCESS;
    case Node::STRING:
        *object = new String(node->text);ASSERT(*object);
        return SUCCESS;
    case Node::QUOTE:
        *object = new Quote(node->text);ASSERT(*object);
        return SUCCESS;
    case Node::SYMBOL:
        *object = new Variable(node->text);ASSERT(*object);
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}

int Translator::translateDefinition(Node* node, Object** object)
{
    if (node->nodes.size() != 3) return SYNTAX_ERROR;
    Node* symbol = node->nodes[1];
    if (symbol->type != Node::SYMBOL) return SYNTAX_ERROR;
    Variable* variable = new Variable(symbol->text);ASSERT(variable);
    Node* argument = node->nodes[2];
    Object* argumentObject;
    if (translate(argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
    *object = new Definition(variable, argumentObject);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateIf(Node* node, Object** object)
{
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
    *object = new SpecialIf(predicate, consequent, alternative);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateCond(Node* node, Object** object)
{
    Clauses* clauses = new Clauses;ASSERT(clauses);
    Objects* elseActions = NULL;
    for (Node::Nodes::size_type i = 1; i < node->nodes.size(); i++)
    {
        Node* n = node->nodes[i];
        if (n->nodes.size() < 2) return SYNTAX_ERROR;
        if (i == node->nodes.size() - 1 && n->nodes[0]->type == Node::SYMBOL && n->nodes[0]->text == "else")
        {
            elseActions = new Objects;ASSERT(elseActions);
            for (Node::Nodes::size_type j = 1; j < n->nodes.size(); j++)
            {
                Object * action;
                int ret = translate(n->nodes[j], &action);
                if (ret != SUCCESS) return ret;
                elseActions->push_back(action);
            }
        }
        else
        {
            // (cond (1 => hoge))
            if (n->nodes.size() == 3 && n->nodes[1]->type == Node::SYMBOL && n->nodes[1]->text == "=>")
            {
                Object* cond;
                int ret = translate(n->nodes[0], &cond);
                if (ret != SUCCESS) return ret;
                Object* action;
                ret = translate(n->nodes[2], &action);
                if (ret != SUCCESS) return ret;
                Objects* arguments = new Objects;ASSERT(arguments);
                arguments->push_back(cond);
                Object* application = new Application(action, arguments);ASSERT(application);
                Objects* actions = new Objects;ASSERT(actions);
                actions->push_back(application);
                Clause* c = new Clause(cond, actions);
                ASSERT(c);
                clauses->push_back(c);
            }
            else
            {
                Object* cond;
                int ret = translate(n->nodes[0], &cond);
                if (ret != SUCCESS) return ret;
                Objects* actions = new Objects;ASSERT(actions);
                for (Node::Nodes::size_type j = 1; j < n->nodes.size(); j++)
                {
                    Object * action;
                    ret = translate(n->nodes[j], &action);
                    if (ret != SUCCESS) return ret;
                    actions->push_back(action);
                }
                Clause* c = new Clause(cond, actions);
                ASSERT(c);
                clauses->push_back(c);
            }
        }
    }
    *object = new Cond(clauses, elseActions);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateAnd(Node* node, Object** object)
{
    Objects* objects = new Objects;ASSERT(objects);
    for (Node::Nodes::size_type i = 1; i < node->nodes.size(); i++)
    {
        Object * object;
        int ret = translate(node->nodes[i], &object);
        if (ret != SUCCESS) return ret;
        objects->push_back(object);
    }
    *object = new And(objects);ASSERT(*object);
    return SUCCESS;
}


int Translator::translateOr(Node* node, Object** object)
{
    Objects* objects = new Objects;ASSERT(objects);
    for (Node::Nodes::size_type i = 1; i < node->nodes.size(); i++)
    {
        Object * object;
        int ret = translate(node->nodes[i], &object);
        if (ret != SUCCESS) return ret;
        objects->push_back(object);
    }
    *object = new Or(objects);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateBegin(Node* node, Object** object)
{
    if (node->nodes.size() <= 1) return SYNTAX_ERROR;
    Objects* objects = new Objects;ASSERT(objects);
    for (Node::Nodes::size_type i = 1; i < node->nodes.size(); i++)
    {
        Object * object;
        int ret = translate(node->nodes[i], &object);
        if (ret != SUCCESS) return ret;
        objects->push_back(object);
    }
    *object = new Begin(objects);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLambda(Node* node, Object** object)
{
    if (node->nodes.size() <= 2) return SYNTAX_ERROR;
    if (node->nodes[1]->type != Node::NODES) return SYNTAX_ERROR;
    Variables* variables = new Variables;ASSERT(variables);
    for (Node::Nodes::size_type i = 0; i < node->nodes[1]->nodes.size(); i++)
    {
        Node* param = node->nodes[1]->nodes[i];
        if (param->type != Node::SYMBOL) return SYNTAX_ERROR;
        Variable* v = new Variable(param->text);
        ASSERT(v);
        variables->push_back(v);
    }

    Objects* body = new Objects;ASSERT(body);
    for (Node::Nodes::size_type i = 2; i < node->nodes.size(); i++)
    {
        Object* o;
        int ret = translate(node->nodes[i], &o);
        if (ret != SUCCESS) return ret;
        body->push_back(o);
    }
    *object = new Lambda(body, variables);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLet(Node* node, Object** object)
{
    if (node->nodes.size() < 3) return SYNTAX_ERROR;
    if (node->nodes[1]->type != Node::NODES) return SYNTAX_ERROR;

    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    Node::Nodes* parameterNodes = &node->nodes[1]->nodes;
    for (Node::Nodes::size_type i = 0; i < parameterNodes->size(); i++)
    {
        Node* parameter = parameterNodes->at(i);
        if (parameter->type != Node::NODES || parameter->nodes.size() != 2) return SYNTAX_ERROR;
        if (parameter->nodes[0]->type != Node::SYMBOL) return SYNTAX_ERROR;
        Variable* v = new Variable(parameter->nodes[0]->text);
        ASSERT(v);
        variables->push_back(v);
        Object* value;
        int ret = translate(parameter->nodes[1], &value);
        if (ret != SUCCESS) return ret;
        values->push_back(value);
    }

    Objects* body = new Objects;ASSERT(body);
    for (Node::Nodes::size_type i = 2; i < node->nodes.size(); i++)
    {
        Object* o;
        int ret = translate(node->nodes[i], &o);
        if (ret != SUCCESS) return ret;
        body->push_back(o);
    }
    *object = new Let(body, variables, values);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLetAsterisk(Node* node, Object** object)
{
    if (node->nodes.size() < 3) return SYNTAX_ERROR;
    if (node->nodes[1]->type != Node::NODES) return SYNTAX_ERROR;

    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    Node::Nodes* parameterNodes = &node->nodes[1]->nodes;
    for (Node::Nodes::size_type i = 0; i < parameterNodes->size(); i++)
    {
        Node* parameter = parameterNodes->at(i);
        if (parameter->type != Node::NODES || parameter->nodes.size() != 2) return SYNTAX_ERROR;
        if (parameter->nodes[0]->type != Node::SYMBOL) return SYNTAX_ERROR;
        Variable* v = new Variable(parameter->nodes[0]->text);
        ASSERT(v);
        variables->push_back(v);
        Object* value;
        int ret = translate(parameter->nodes[1], &value);
        if (ret != SUCCESS) return ret;
        values->push_back(value);
    }

    Objects* body = new Objects;ASSERT(body);
    for (Node::Nodes::size_type i = 2; i < node->nodes.size(); i++)
    {
        Object* o;
        int ret = translate(node->nodes[i], &o);
        if (ret != SUCCESS) return ret;
        body->push_back(o);
    }
    *object = new LetAsterisk(body, variables, values);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateApplication(Node* node, Object** object)
{
    Object* f;
    int ret = translate(node->nodes[0], &f);
    if (ret != SUCCESS) return ret;
    Objects* arguments = new Objects;ASSERT(arguments);
    for (Node::Nodes::size_type i = 1; i < node->nodes.size(); i++)
    {
        Object * object;
        int ret = translate(node->nodes[i], &object);
        if (ret != SUCCESS) return ret;
        arguments->push_back(object);
    }
    *object = new Application(f, arguments);ASSERT(*object);
    return SUCCESS;
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
        if (function->text == "define")
        {
            return translateDefinition(node, object);
        }
        else if (function->text == "if")
        {
            return translateIf(node, object);
        }
        else if (function->text == "begin")
        {
            return translateBegin(node, object);
        }
        else if (function->text == "lambda")
        {
            return translateLambda(node, object);
        }
        else if (function->text == "and")
        {
            return translateAnd(node, object);
        }
        else if (function->text == "or")
        {
            return translateOr(node, object);
        }
        else if (function->text == "cond")
        {
            return translateCond(node, object);
        }
        else if (function->text == "let")
        {
            return translateLet(node, object);
        }
        else if (function->text == "let*")
        {
            return translateLetAsterisk(node, object);
        }
        else
        {
            return translateApplication(node, object);
        }
    }
    else
    {
        return translateApplication(node, object);
    }
    return SYNTAX_ERROR;
}
