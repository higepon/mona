#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "Node.h"
#include "Object.h"
#include "Procedure.h"
#include "Number.h"
#include "String.h"
#include "Quote.h"
#include "And.h"
#include "Or.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "Cond.h"
#include "Let.h"
#include "LetAsterisk.h"
#include "Macro.h"

namespace monash {

class Macro;
typedef std::map<std::string, Macro*> Macros;

class Translator
{
public:
    Translator();
    virtual ~Translator();

public:
    enum
    {
        SYNTAX_ERROR,
        SUCCESS
    };
    int translate(Node* node, Object** object);
//    int translateMacro(Node* defineSyntax, Node* from, Node** to);
    int translateDefineSyntax(Node* node);
    Node* expandMacroIfMatch(const std::string& name, Node* node);
    void reset() { expanded_ = NULL;}

    Node* getExpandedNode() const { return expanded_; }
private:
    int translatePrimitive(Node* node, Object** object);
    int translateDefinition(Node* node, Object** object);

    int translateIf(Node* node, Object** object);
    int translateAnd(Node* node, Object** object);
    int translateOr(Node* node, Object** object);
    int translateCond(Node* node, Object** object);
    int translateBegin(Node* node, Object** object);
    int translateLambda(Node* node, Object** object);
    int translateLet(Node* node, Object** object);
    int translateLetAsterisk(Node* node, Object** object);
    int translateApplication(Node* node, Object** object);
//    int expandMacro(Node* from, BindMap& bindMap);
    int expandMacroInternal(Node* from, BindMap& bindMap);
//    bool matchMacro(const std::string& name, Node* node);

    Node* expandMacro(Macro* macro, Node* matchedPattern, Node* from);

    Node* expanded_;
    Macros macros_;
};

}; // namespace monash

#endif // __TRANSLATOR_H__
