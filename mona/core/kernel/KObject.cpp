#include "KObject.h"
#include "kernel.h"
#include "global.h"

KObject::KObject() : referanceCount_(0)
{
}

KObject::~KObject()
{
}

intptr_t KObject::getReferanceCount() const
{
    return referanceCount_;
}

void KObject::addRef()
{
    referanceCount_++;
}

bool KObject::releaseRef()
{
    referanceCount_--;
    ASSERT(referanceCount_ >= 0);
    return referanceCount_ == 0;
}

void KObject::setOwner(Thread* owner)
{
    owner_ = owner;
}

Thread* KObject::getOwner() const
{
    return owner_;
}
