#ifndef __SEXP_H__
#define __SEXP_H__

#include <vector>
#include <string>
#include <stdio.h>
#include <map>
#include <stdio.h>
#include "Tokenizer.h"
#include "Error.h"
#include "Parser.h"

std::string load(const char* file);
namespace monash {

class ZSExp
{
public:
    ZSExp() {}
    virtual ~ZSExp() {}

    enum
    {
        NUMBER,
        CHARCTER,
        STRING,
        SYMBOL,
        PAIR,
    };

    bool isNumber() const { return getType() == NUMBER; }
    bool isCharcter() const { return getType() == CHARCTER; }
    bool isString() const { return getType() == STRING; }
    bool isPair() const { return getType() == PAIR; }

    virtual ZSExp* eval() = 0;
    virtual int getType() const = 0;
    virtual std::string toString() = 0;
};

class ZSelfEvaluating : public ZSExp
{
public:
    ZSelfEvaluating() {}
    virtual ~ZSelfEvaluating() {}
    virtual ZSExp* eval() { return this; }
};

class ZNumber : public ZSelfEvaluating
{
public:
    ZNumber(int value) : value_(value) {}
    virtual ~ZNumber() {}

    int getValue() const { return value_; }
    void setValue(int value) { value_ = value; }
    int getType() const { return NUMBER; }
    std::string toString();

protected:
    int value_;
};

class ZCharactor : public ZSelfEvaluating
{
public:
    ZCharactor(char value) : value_(value) {}
    virtual ~ZCharactor() {}

    char getValue() const { return value_; }
    void setValue(char value) { value_ = value; }
    int getType() const { return CHARCTER; }
    std::string toString()
    {
        char buf[32];
        sprintf(buf, "%c", value_);
        return std::string(buf);
    }

protected:
    char value_;
};

class ZString : public ZSelfEvaluating
{
public:
    ZString(const std::string& value) : value_(value) {}
    virtual ~ZString() {}

    std::string getValue() const { return value_; }
    void setValue(const std::string& value) { value_ = value; }
    int getType() const { return STRING; }
    std::string toString() { return "\"" + value_ + "\""; }

protected:
    std::string value_;
};

class ZSymbol : public ZSExp
{
public:
    ZSymbol(const std::string& value) : value_(value) {}
    virtual ~ZSymbol() {}

    std::string getValue() const { return value_; }
    void setValue(const std::string& value) { value_ = value; }
    int getType() const { return SYMBOL; }
    std::string toString() { return value_; }
    virtual ZSExp* eval() { return NULL;} 

protected:
    std::string value_;
};

class ZPair : public ZSExp
{
public:
    ZPair(ZSExp* car, ZSExp* cdr) : car_(car), cdr_(cdr) {}
    virtual ~ZPair() {}

    ZSExp* getCar() const { return car_; }
    ZSExp* getCdr() const { return cdr_; }
    void setCar(ZSExp* car) { car_ = car; }
    void setCdr(ZSExp* cdr) { cdr_ = cdr; }
    int getType() const { return PAIR; }
    std::string toString();
    std::string toStringCdr();
    virtual ZSExp* eval() { return NULL;} 

protected:
    ZSExp* car_;
    ZSExp* cdr_;
};


class SExp;
typedef std::vector<SExp*> SExps;

#define N(n)         sexp->sexps[n]
#define NN(i, j)     sexp->sexps[i]->sexps[j]
#define NNN(i, j, k) sexp->sexps[i]->sexps[j]->sexps[k]
#define L()          sexp->sexps.size()
#define LL(n)        sexp->sexps[n]->sexps.size()


class BindObject;
typedef std::map<std::string, BindObject> BindMap;

class BindObject
{
public:
    BindObject() : sexp(NULL) {}
public:
    SExp* sexp;
    SExps sexps;
};


class SExp
{
public:
    SExp(int type) : type(type), lineno(0) {}
    ~SExp() {}

    std::string toString();
    std::string toSExpString();
    void print(int depth = 0);
    bool equals(SExp* sexp);
    SExp* clone() const;

    ZSExp* toZSExp();

    bool isSExps()  const { return type == SEXPS; }
    bool isNumber() const { return type == NUMBER; }
    bool isSymbol() const { return type == SYMBOL; }
    bool isString() const { return type == STRING; }
    bool isQuote()  const { return type == QUOTE; }
    bool isChar()  const { return type == CHAR; }
    bool isMatchAllKeyword() const { return isSymbol() && text.find("...") != std::string::npos; }

    static SExp* fromString(const std::string& text);
    static void extractBindings(SExp* m, SExp* n, BindMap& bindMap);

    int foreachSExp(SExp* root, bool (SExp::*match)() const, int (SExp::*func)(SExp* root, SExp* sexp));
    int foreachSExps(SExp* root, int (SExp::*f)(SExp*, SExp*));
#if 0
    int execLoadSyntax(SExp* root, SExp* sexp);
    int execLoadSyntaxes();
#endif
    SExps sexps;
    std::string text;
    int value;
    int type;
    uint32_t lineno;

    enum
    {
        SEXPS,
        NUMBER,
        SYMBOL,
        STRING,
        QUOTE,
        CHAR,
    };

private:
    static bool equalsInternal(SExp* m, SExp* n);
    void toStringInternal(uint32_t depth, std::string& s);
    void toSExpStringInternal(std::string& s);
    std::string typeToString();
    std::string typeToRawString();
};

}; // namespace monash

#endif // __SEXP_H__
