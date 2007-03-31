#include "ExtRepParser.h"

using namespace monash;

#define SYNTAX_ERROR(...) printf(__VA_ARGS__);fflush(stdout);

ExtRepParser::ExtRepParser(Scanner* scanner) : scanner_(scanner)
{
}

ExtRepParser::~ExtRepParser()
{
}

Object* ExtRepParser::parse()
{
    nextToken();
    return parseDatum();
}

SToken* ExtRepParser::nextToken()
{
    return (token_ = scanner_->getToken());
}


// private

// <datum> => <simple datum> | <compound datum>
// <simple datum> => <boolean> | <number> | <character> | <string>
//                 | <symbol>
Object* ExtRepParser::parseDatum()
{
    if (token_ == NULL) return SCM_EOF;

    else if (token_->type == SToken::COMMENT)
    {
        for (;;)
        {
            nextToken();
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
        return parseSimpleDatum();
    default:
        return parseCompoundDatum();
    }
}

// <simple datum> => <boolean> | <number> | <character> | <string>
//                 | <symbol>
Object* ExtRepParser::parseSimpleDatum()
{
    switch(token_->type)
    {
    case SToken::BOOLEAN:
        if (token_->integer == 1)
        {
            return new True;
        }
        else
        {
            return new False;
        }
    case SToken::NUMBER:
        return new Number(token_->integer);
        break;
    case SToken::CHARCTER:
        return new Charcter("#\\" + token_->text);
    case SToken::STRING:
        return new SString(token_->text);
    case SToken::VARIABLE:
    case SToken::KEYWORD:
        return new RiteralConstant(token_->text);
    default:
        SYNTAX_ERROR("invalid simple datum %s:%s\n", token_->typeString().data(), token_->valueString().data());
        break;
    }
    return NULL;
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
    return NULL;
}

// <vector> => #(<datum>*)
Object* ExtRepParser::parseVector()
{
    if (token_->type != SToken::VECTOR_START)
    {
        SYNTAX_ERROR("vector expected, but got %s %s", token_->typeString().data(), token_->valueString().data());
        return NULL;
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
    Vector* v = new Vector(objects->size());
    for (uint32_t i = 0; i < objects->size(); i++)
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
                Pair* start = new Pair(SCM_NIL, SCM_NIL);
                Pair* p = start;
                for (int i = 0; i < objects->size(); i++)
                {
                    p->setCar(objects->get(i));
                    if (i == objects->size() - 1)
                    {
                        p->setCdr(o);
                    }
                    else
                    {
                        Pair* tmp = new Pair(SCM_NIL, SCM_NIL);
                        p->setCdr(tmp);
                        p = tmp;
                    }
                }
                return start;
            }
            else if (token_->type == SToken::RIGHT_PAREN)
            {
                if (objects->size() == 0) return SCM_NIL;
                Pair* start = new Pair(SCM_NIL, SCM_NIL);
                Pair* p = start;
                for (int i = 0; i < objects->size(); i++)
                {
                    p->setCar(objects->get(i));
                    if (i != objects->size() -1)
                    {
                        Pair* tmp = new Pair(SCM_NIL, SCM_NIL);
                        p->setCdr(tmp);
                        p = tmp;
                    }
                }
                return start;
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
            return new Pair(new RiteralConstant("quote"), new Pair(parseDatum(), SCM_NIL));
        case SToken::BACK_QUOTE:
        case SToken::CAMMA:
        case SToken::CAMMA_AT:
            SYNTAX_ERROR("soory not supported\n");
            return NULL;
        }
    }
    return NULL;
}