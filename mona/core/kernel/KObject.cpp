#include "KObject.h"
#include "kernel.h"
#include "global.h"

KObject::KObject() : referanceCount_(0)
{
}

KObject::~KObject()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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
