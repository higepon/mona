#include "Vector.h"

using namespace monash;
using namespace std;

Vector::Vector(uint32_t size, uint32_t lineno /* = 0 */) : size_(size), lineno_(lineno)
{
    values_ = new Objects(size);
}

Vector::Vector(uint32_t size, Object* o, uint32_t lineno /* = 0 */) : size_(size), lineno_(lineno)
{
    values_ = new Objects();
    for (uint32_t i = 0; i < size; i++)
    {
        values_->push_back(o);
    }

}

Vector::Vector(Objects* values, uint32_t lineno /* = 0 */) : values_(values), size_(values->size()), lineno_(lineno)
{
}

Vector::~Vector()
{
}

Object* Vector::get(uint32_t index)
{
    if (index > size_  || index < 0) return NULL;
    return values_->at(index);
}

bool Vector::set(uint32_t index, Object* o)
{
    if (index > size_ - 1 || index < 0) return false;
    values_->erase(values_->begin() + index);
    values_->insert(values_->begin() + index, o);
    return true;
}

string Vector::toString()
{
    return "vector";
}

string Vector::toStringValue()
{
    string ret = "#(";
    for (Objects::const_iterator p = values_->begin(); p != values_->end(); ++p)
    {
        ret += (*p)->toStringValue();
        if (p != values_->end() - 1) ret += " ";
    }
    ret += ")";
    return ret;
}

int Vector::type() const
{
    return Object::VECTOR;
}

Object* Vector::eval(Environment* env)
{
    return this;
}

bool Vector::eqv(Object* o)
{
    if (!o->isVector()) return false;
    Vector* s = (Vector*)o;
    return this == s;
}

bool Vector::eq(Object* o)
{
    return false;
}
