#include "Quote.h"

using namespace monash;

Quote::Quote(Node* node, uint32_t lineno /* = 0 */) : node_(node), lineno_(lineno)
{
}

Quote::~Quote()
{
}

std::string Quote::toString()
{
    return "quote: \'";
}

std::string Quote::toStringValue()
{
    return node_->toString();
}

int Quote::type() const
{
    return Object::QUOTE;
}

Quote* Quote::car()
{
    if (node_->isNodes())
    {
        return new Quote(node_->nodes[0], lineno_);
    }
    return NULL;
}

Quote* Quote::cdr()
{
    if (node_->isNodes())
    {
        return new Quote(node_->nodes[1], lineno_);
    }
    return NULL;
}

Object* Quote::eval(Environment* env)
{
    return this;
//    return evalSequence(objects_, env); // different from SICP's
}
