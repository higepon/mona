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

void KObject::setReferance()
{
    this->referanceCount++;
}

void KObject::cancelReferance()
{
    this->referanceCount--;
}

