#include "KObject.h"

KObject::KObject()
{
    this->referanceCount = 0;
}

KObject::~KObject()
{
}

intptr_t KObject::getReferanceCount() const
{
    return this->referanceCount;
}

void KObject::addRef()
{
    this->referanceCount++;
}

bool KObject::releaseRef()
{
    this->referanceCount--;
    return this->referanceCount == 0;
}

