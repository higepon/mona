#ifndef _HIGEPOS_VECTOR_
#define _HIGEPOS_VECTOR_

/*!
    \file  HVector.h
    \brief class HVector

    class HVector this is simple vector.
    I hope this class will be replaced by real vector

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/10/22 update:$Date$
*/
template <class T> class HVector {

  public:
    HVector();
    HVector(unsigned long size);
    ~HVector();
    void add(T element);
    T get(unsigned long index);
    T remove(unsigned long index);
    unsigned long size() const;
  private:
    T* data_;
    unsigned long size_;

};

/*!
    \brief constructor

    constructor default size is 5

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HVector<T>::HVector() {

    /* default size of HVector is 5*/
    size_ = 4;
    data_ = new T[size_];
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of vector

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HVector<T>::HVector(unsigned long size) {

    if (size > 0) {
        data_ = new T[size];
    }

    size_ = size;
    return;
}

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HVector<T>::~HVector() {

    /* release memory */
    delete[] data_;
    return;
}

template <class T> void HVector<T>::add(T element) {
    return;
}

template <class T> T HVector<T>::get(unsigned long index) {
    return (T)0;
}

/*!
    \brief size

    return size of vector

    \return size of vector

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> unsigned long HVector<T>::size() const {
    return size_;
}

template <class T> T HVector<T>::remove(unsigned long index) {


}

#endif
