#include "KObject.h"

const int KObject::KOBJECT = 0;
const int KObject::THREAD  = 1;

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

int KObject::getType() const
{
    return KOBJECT;
}
