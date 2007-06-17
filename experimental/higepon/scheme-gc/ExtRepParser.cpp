#include "scheme.h"

using namespace monash;

#define SYNTAX_ERROR(...) printf(__VA_ARGS__);printf(" at %s:%d\n", scanner_->getFileName().data(), scanner_->getLineNo());fflush(stdout);
#define NEW(symbol, value) new symbol(value, scanner_->getLineNo())
#define NEW2(symbol, v1, v2) new symbol(v1, v2, scanner_->getLineNo())

ExtRepParser::ExtRepParser(Scanner* scanner) : scanner_(scanner)
{
}

ExtRepParser::~ExtRepParser()
{
}

Object* ExtRepParser::parse()
{
    nextToken();
    Object* ret = parseDatum();
    findSelfCall(ret);
    return ret;
}

void ExtRepParser::findSelfCall(Object* o)
{
    if (!o->isPair()) return;
    Pair* p = (Pair*)o;
    if (!p->getCar()->isRiteralConstant()) return;
    RiteralConstant* c = (RiteralConstant*)(p->getCar());
    if (c->text() != "define") return;
    if (!p->getCdr()->isPair()) return;
    Pair* p2 = (Pair*)p->getCdr();
    if (!p2->getCar()->isRiteralConstant()) return;
    RiteralConstant* c2 = (RiteralConstant*)p2->getCar();
    ::util::String name = c2->text();

    if (!p2->getCdr()->isPair()) return;
    Pair* p3 = (Pair*)p2->getCdr();
    findNameCall(p3, name);
}

void ExtRepParser::findNameCall(Pair* p, const ::util::String& name)
{



}

SToken* ExtRepParser::nextToken()
{
    return (token_ = scanner_->getToken());
}

// private

// <datum> => <simple datum> | <compound datum>
// <simple datum> => <boolean> | <number> | <character> | <string> | <regexp>
//                 | <symbol>
Object* ExtRepParser::parseDatum()
{
    if (token_ == NULL)
    {
        return SCM_EOF;
    }
    else if (token_->type == SToken::COMMENT)
    {
        for (;;)
        {
            nextToken();
            if (token_ == NULL) return SCM_EOF;
            if (token_->type != SToken::COMMENT) break;
        }
    }

    switch(token_->type)
    {
    case SToken::BOOLEAN:
    case SToken::NUMBER:
    case SToken::CHARCTER:
    case SToken::STRING:
    case SToken::VARIABLE:
    case SToken::KEYWORD:
    case SToken::REGEXP:
        return parseSimpleDatum();
    default:
        return parseCompoundDatum();
    }
}

// <simple datum> => <boolean> | <number> | <character> | <string> | <regexp>
//                 | <symbol>
Object* ExtRepParser::parseSimpleDatum()
{
    switch(token_->type)
    {
    case SToken::BOOLEAN:
        if (token_->integer == 1)
        {
            return SCM_TRUE;
        }
        else
        {
            return SCM_FALSE;
        }
    case SToken::NUMBER:
        return NEW(Number, token_->integer);
        break;
    case SToken::CHARCTER:
        return NEW(Charcter, "#\\" + token_->text);
    case SToken::STRING:
        return NEW(SString, token_->text);
    case SToken::VARIABLE:
    case SToken::KEYWORD:
        return NEW(RiteralConstant, token_->text);
    case SToken::REGEXP:
        return new SRegexp(token_->text, token_->integer == 1 ? true : false, scanner_->getLineNo());
    default:
        SYNTAX_ERROR("invalid simple datum %s:%s\n", token_->typeString().data(), token_->valueString().data());
        break;
    }
    return SCM_EOF;
}

// <compound datum> => <list> | <vector>
// <list> => (<datum>*) | (<datum>+ . <datum>) | <abbreviation>
// <abbreviation> => <abbrev prefix> <datum>
// <abbrev prefix> => ' | ` | , | ,@
// <vector> => #(<datum>*)
Object* ExtRepParser::parseCompoundDatum()
{
    switch(token_->type)
    {
    case SToken::VECTOR_START:
        return parseVector();
    case SToken::LEFT_PAREN:
    case SToken::SINGLE_QUOTE:
    case SToken::BACK_QUOTE:
    case SToken::CAMMA:
    case SToken::CAMMA_AT:
        return parseList();
    default:
        SYNTAX_ERROR("list or vector should be here, but got %s %s", token_->typeString().data(), token_->valueString().data());
    }
    return SCM_EOF;
}

// <vector> => #(<datum>*)
Object* ExtRepParser::parseVector()
{
    if (token_->type != SToken::VECTOR_START)
    {
        SYNTAX_ERROR("vector expected, but got %s %s", token_->typeString().data(), token_->valueString().data());
        return SCM_EOF;
    }
    Objects* objects = new Objects;
    for (;;)
    {
        nextToken();
        if (token_->type == SToken::RIGHT_PAREN)
        {
            break;
        }
        objects->add(parseDatum());
    }
    Vector* v = NEW(Vector, objects->size());
    for (int i = 0; i < objects->size(); i++)
    {
        v->set(i, objects->get(i));
    }
    return v;
}

// <list> => (<datum>*) | (<datum>+ . <datum>) | <abbreviation>
// <abbreviation> => <abbrev prefix> <datum>
// <abbrev prefix> => ' | ` | , | ,@
Object* ExtRepParser::parseList()
{
    // (<datum>*) | (<datum>+ . <datum>)
    if (token_->type == SToken::LEFT_PAREN)
    {
        Objects* objects = new Objects;
        for (;;)
        {
            nextToken();
            if (token_->type == SToken::COMMENT)
            {
                for (;;)
                {
                    nextToken();
                    if (token_ == NULL) return SCM_EOF;
                    if (token_->type != SToken::COMMENT) break;
                }
            }

            if (token_->type == SToken::PERIOD)
            {
                nextToken();
                Object* o = parseDatum();
                nextToken();
                if (token_->type != SToken::RIGHT_PAREN)
                {
                    SYNTAX_ERROR("invalid list, . position is wrong");
                    return NULL;
                }
                if (objects->size() == 0)
                {
                    SYNTAX_ERROR("invalid list, . position is wrong");
                    return NULL;
                }

                Pair* ret;
                // (objects[0], objects[1] ... . o)
                SCM_LIST_CONS(objects, o, ret, scanner_->getLineNo());
                return ret;
            }
            else if (token_->type == SToken::RIGHT_PAREN)
            {
                if (objects->size() == 0) return SCM_NIL;
                Pair* ret;
                SCM_LIST(objects, ret, scanner_->getLineNo());
                return ret;
            }
            else
            {
                objects->add(parseDatum());
            }
        }
    }
    else
    {
        // <abbreviation> => <abbrev prefix> <datum>
        // <abbrev prefix> => ' | ` | , | ,@
        switch(token_->type)
        {
        case SToken::SINGLE_QUOTE:
            nextToken();
            return NEW2(Pair, new RiteralConstant("quote"), new Pair(parseDatum(), SCM_NIL));
        case SToken::CAMMA_AT:
        case SToken::BACK_QUOTE:
        case SToken::CAMMA:

            SYNTAX_ERROR("soory not supported\n");
            return NULL;
        }
    }
    return NULL;
}
