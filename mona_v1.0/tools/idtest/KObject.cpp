#include "KObject.h"

KObject::KObject()
{
    this->referanceCount = 0;
}

KObject::~KObject()
{
}

int KObject::checkSecurity(Thread* thread)
{
    /* default inplementation */
    return 0;
}

int KObject::getReferanceCount() const
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
