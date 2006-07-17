#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "Node.h"
#include "Object.h"
#include "Procedure.h"
#include "Number.h"
#include "String.h"
#include "Quote.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"

namespace monash {

class Translator
{
public:
    Translator();
    virtual ~Translator();

public:
    static int translate(Node* node, Object** object);
    static int translatePrimitive(Node* node, Object** object);
    enum
    {
        SYNTAX_ERROR,
        SUCCESS
    };

};

}; // namespace monash

#endif // __TRANSLATOR_H__
