#ifndef _HIGEPOS_VECTOR_
#define _HIGEPOS_VECTOR_

template <class T> class Vector {

  public:
    Vector();
    Vector(unsigned long size);
    ~Vector();
    void add(T element);
    T get(unsigned long index);
    T remove(unsigned long index);
    unsigned long size();
};
#endif
