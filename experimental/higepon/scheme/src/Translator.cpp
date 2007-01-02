#include "Translator.h"

using namespace monash;
using namespace std;

#define N(n)         node->nodes[n]
#define NN(i, j)     node->nodes[i]->nodes[j]
#define NNN(i, j, k) node->nodes[i]->nodes[j]->nodes[k]
#define L()          node->nodes.size()
#define LL(n)        node->nodes[n]->nodes.size()

Translator::Translator()
{
}

Translator::~Translator()
{
}

Node* Translator::expandMacroIfMatch(const std::string& name, Node* node)
{
//    printf("%s %s:%d %s\n", __func__, __FILE__, __LINE__, node->toString().c_str());fflush(stdout);// debug
//     for (Macros::iterator i = macros_.begin(); i != macros_.end(); ++i)
//     {
//         printf("macro:[%s]\n", (*i).first.c_str());fflush(stdout);
//     }


//     printf("%s %s:%d[ %s : %d]\n", __func__, __FILE__, __LINE__, name.c_str(), macros_.size());fflush(stdout);// debug
    Macros::iterator p = macros_.find(name);
    if (p == macros_.end()) return NULL;


    Macro* m = (*p).second;

    // todo Macro::Patterを返すべきでは?
    Node* matchedPattern = m->match(name, node);
    if (NULL == matchedPattern) return NULL;
    return expandMacro(m, matchedPattern, node);
}

int Translator::expandMacroInternal(Node* from, BindMap& bindMap)
{
    for (Nodes::size_type i = 0; i < from->nodes.size(); ++i)
    {
        Node* f = from->nodes[i];
        if (f->isSymbol()) printf("<%s>\n", f->text.c_str());

        if (f->isSymbol() && bindMap.find(f->text) != bindMap.end())
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            if (f->text == "...")
            {
                BindObject b = bindMap[f->text];
                for (Nodes::size_type j = 0; j < b.nodes.size(); ++j)
                {
  //               printf("**********************:::%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//                 printf(b.nodes[j]->toString().c_str());
                    from->nodes[i + j] = b.nodes[j];
                }
            }
            else
            {
                from->nodes[i] = bindMap[f->text].node;
            }
        }
        else if (f->isNodes())
        {
            expandMacroInternal(f, bindMap);
        }
    }
    return SUCCESS;
}


// bool Translator::matchMacro(const string& name, Node* node)
// {
//     Macros::iterator p = macros_.find(name);
//     if (p == macros_.end()) return false;
//     Macro* m = (*p).second;

// }

// int Translator::expandMacro2(Node* from, BindMap& bindMap)
// {
//     for (Nodes::size_type i = 0; i < from->nodes.size(); ++i)
//     {
//         Node* f = from->nodes[i];
//         if (f->isSymbol()) printf("<%s>\n", f->text.c_str());

//         if (f->isSymbol() && bindMap.find(f->text) != bindMap.end())
//         {
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             from->nodes[i] = bindMap[f->text];
//         }
//         else if (f->isNodes())
//         {
//             expandMacro(f, bindMap);
//         }
//     }
//     return SUCCESS;
// }

Node* Translator::expandMacro(Macro* macro, Node* matchedPattern, Node* from)
{
    BindMap bindMap;
    Node::extractBindings(matchedPattern, from, bindMap);

//     for (BindMap::const_iterator p = bindMap.begin(); p != bindMap.end(); ++p)
//     {
//         printf("%s => %s\n"
//                , p->first.c_str()
//                , p->second->toString().c_str());fflush(stdout);
//     }

    Node* expanded = macro->patterns[matchedPattern]->clone();//(*(macro->patterns.find(matchedPattern)))->clone();
    expandMacroInternal(expanded, bindMap);
    return expanded;
}


// int Translator::translateMacro(Node* defineSyntax, Node* from, Node** to)
// {
//     // マッチしているかは事前にチェックされるはず

//     // syntax-rules の本体だけ取り出す
// //    definition->print();
//     string macroName = defineSyntax->nodes[1]->text;
//     Node* pattern = defineSyntax->nodes[2]->nodes[2]->nodes[0];
//     Node* definition  = defineSyntax->nodes[2]->nodes[2]->nodes[1];
//     Macro* macro = new Macro(macroName);
//     macro->addPattern(pattern, definition);
//     macros_[macroName] = macro;

//     BindMap bindMap;
//     Node::extractBindings(pattern, from, bindMap);

//     for (BindMap::const_iterator p = bindMap.begin(); p != bindMap.end(); ++p)
//     {
//         printf("%s => %s\n"
//                , p->first.c_str()
//                , p->second->toString().c_str());fflush(stdout);
//     }

//     Node* translated = definition->clone();
//     expandMacro(translated, bindMap);


//     // macro dump
//     for (Macros::iterator p = macros_.begin(); p != macros_.end(); ++p)
//     {
//         Macro* macro = (*p).second;
//         for (Macro::Patterns::iterator q = macro->patterns.begin(); q != macro->patterns.end(); ++q)
//         {
//             printf("%s => %s\n", (*q).first->toString().c_str(), (*q).second->toString().c_str());

//         }
//     }


//     *to = translated;
//     return SUCCESS;
// }

int Translator::translateDefineSyntax(Node* node)
{
    if (L() != 3) return SYNTAX_ERROR;
    if (!N(1)->isSymbol()) return SYNTAX_ERROR;
    if (!N(2)->isNodes() || LL(2) < 3) return SYNTAX_ERROR;
    if (!NN(2, 0)->isSymbol() || NN(2, 0)->text != "syntax-rules") return SYNTAX_ERROR;
    if (!NN(2, 1)->isNodes()) return SYNTAX_ERROR;
    // macro name
    Macro* macro = new Macro(N(1)->text);

    // store reserved words
    for (Nodes::const_iterator p = NN(2, 1)->nodes.begin(); p != NN(2, 1)->nodes.end(); ++p)
    {
        Node* n = (*p);
        if (!n->isSymbol()) return SYNTAX_ERROR;
        macro->reservedWords.push_back(n->text);
    }
    // store pattern / definition
    for (Nodes::size_type i = 2; i < LL(2); ++i)
    {
        Node* n = NN(2, i);
        if (!n->isNodes() || n->nodes.size() != 2) return SYNTAX_ERROR;
        macro->addPattern(n->nodes[0], n->nodes[1]);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    }
    macros_[macro->name] = macro;
    return SUCCESS;
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
    if (L() != 3) return SYNTAX_ERROR;
    Node* symbol = N(1);
    if (symbol->type != Node::SYMBOL) return SYNTAX_ERROR;
    Variable* variable = new Variable(symbol->text);ASSERT(variable);
    Node* argument = N(2);
    Object* argumentObject;
    if (translate(argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
    *object = new Definition(variable, argumentObject);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateIf(Node* node, Object** object)
{
    if (L() != 4) return SYNTAX_ERROR;
    Object* predicate;
    Object* consequent;
    Object* alternative;
    int ret = translate(N(1), &predicate);
    if (ret != SUCCESS) return ret;
    ret = translate(N(2), &consequent);
    if (ret != SUCCESS) return ret;
    ret = translate(N(3), &alternative);
    if (ret != SUCCESS) return ret;
    *object = new SpecialIf(predicate, consequent, alternative);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateCond(Node* node, Object** object)
{
    Clauses* clauses = new Clauses;ASSERT(clauses);
    Objects* elseActions = NULL;
    for (Nodes::size_type i = 1; i < L(); i++)
    {
        Node* n = node->nodes[i];
        if (n->nodes.size() < 2) return SYNTAX_ERROR;
        if (i == L() - 1 && n->nodes[0]->type == Node::SYMBOL && n->nodes[0]->text == "else")
        {
            elseActions = new Objects;ASSERT(elseActions);
            for (Nodes::size_type j = 1; j < n->nodes.size(); j++)
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
                for (Nodes::size_type j = 1; j < n->nodes.size(); j++)
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
    for (Nodes::size_type i = 1; i < L(); i++)
    {
        Object * object;
        int ret = translate(N(i), &object);
        if (ret != SUCCESS) return ret;
        objects->push_back(object);
    }
    *object = new And(objects);ASSERT(*object);
    return SUCCESS;
}


int Translator::translateOr(Node* node, Object** object)
{
    Objects* objects = new Objects;ASSERT(objects);
    for (Nodes::size_type i = 1; i < L(); i++)
    {
        Object * object;
        int ret = translate(N(i), &object);
        if (ret != SUCCESS) return ret;
        objects->push_back(object);
    }
    *object = new Or(objects);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateBegin(Node* node, Object** object)
{
    if (L() <= 1) return SYNTAX_ERROR;
    Objects* objects = new Objects;ASSERT(objects);
    for (Nodes::size_type i = 1; i < L(); i++)
    {
        Object * object;
        int ret = translate(N(i), &object);
        if (ret != SUCCESS) return ret;
        objects->push_back(object);
    }
    *object = new Begin(objects);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLambda(Node* node, Object** object)
{
    if (L() <= 2) return SYNTAX_ERROR;
    if (N(1)->type != Node::NODES) return SYNTAX_ERROR;
    Variables* variables = new Variables;ASSERT(variables);
    for (Nodes::size_type i = 0; i < N(1)->nodes.size(); i++)
    {
        Node* param = NN(1, i);
        if (param->type != Node::SYMBOL) return SYNTAX_ERROR;
        Variable* v = new Variable(param->text);
        ASSERT(v);
        variables->push_back(v);
    }

    Objects* body = new Objects;ASSERT(body);
    for (Nodes::size_type i = 2; i < L(); i++)
    {
        Object* o;
        int ret = translate(N(i), &o);
        if (ret != SUCCESS) return ret;
        body->push_back(o);
    }
    *object = new Lambda(body, variables);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLet(Node* node, Object** object)
{
    if (L() < 3) return SYNTAX_ERROR;
    if (N(1)->type != Node::NODES) return SYNTAX_ERROR;

    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    Nodes* parameterNodes = &N(1)->nodes;
    for (Nodes::size_type i = 0; i < parameterNodes->size(); i++)
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
    for (Nodes::size_type i = 2; i < L(); i++)
    {
        Object* o;
        int ret = translate(N(i), &o);
        if (ret != SUCCESS) return ret;
        body->push_back(o);
    }
    *object = new Let(body, variables, values);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLetAsterisk(Node* node, Object** object)
{
    if (L() < 3) return SYNTAX_ERROR;
    if (N(1)->type != Node::NODES) return SYNTAX_ERROR;

    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    Nodes* parameterNodes = &N(1)->nodes;
    for (Nodes::size_type i = 0; i < parameterNodes->size(); i++)
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
    for (Nodes::size_type i = 2; i < L(); i++)
    {
        Object* o;
        int ret = translate(N(i), &o);
        if (ret != SUCCESS) return ret;
        body->push_back(o);
    }
    *object = new LetAsterisk(body, variables, values);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateApplication(Node* node, Object** object)
{
    Object* f;
    int ret = translate(N(0), &f);
    if (ret != SUCCESS) return ret;
    Objects* arguments = new Objects;ASSERT(arguments);
    for (Nodes::size_type i = 1; i < L(); i++)
    {
        Object * object;
        int ret = translate(N(i), &object);
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

    if (L() <= 0) return SYNTAX_ERROR;

    Node* function = N(0);
    if (function->type == Node::SYMBOL)
    {
        string functionName = function->text;

        Node* expanded = expandMacroIfMatch(functionName, node);
        if (expanded != NULL)
        {
            printf("$$$$$$$$$$$$$$$$expand=%s\n", expanded->toString().c_str());
            return translate(expanded, object);
        }
        else if (functionName == "define-syntax")
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debugs
            return translateDefineSyntax(node);
        }
        else if (functionName == "define")
        {
            return translateDefinition(node, object);
        }
        else if (functionName == "if")
        {
            return translateIf(node, object);
        }
        else if (functionName == "begin")
        {
            return translateBegin(node, object);
        }
        else if (functionName == "lambda")
        {
            return translateLambda(node, object);
        }
        else if (functionName == "and")
        {
            return translateAnd(node, object);
        }
        else if (functionName == "or")
        {
            return translateOr(node, object);
        }
        else if (functionName == "cond")
        {
            return translateCond(node, object);
        }
        else if (functionName == "let")
        {
            return translateLet(node, object);
        }
        else if (functionName == "let*")
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
