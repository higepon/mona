#ifndef _MONA_VECTOR_
#define _MONA_VECTOR_

#include<monaTypes.h>

/*!
    \file  HVector.h
    \brief class HVector

    class HVector this is simple vector.
    I hope this class will be replaced by real vector.

    !!! in first use, you should debug this class. !!!

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/10/22 update:$Date$
*/
template <class T> class HVector {

  public:
    HVector();
    HVector(size_t size);
    HVector(size_t size, size_t increase);
    ~HVector();
    void add(T element);
    T get(size_t index) const;
    T operator[](size_t index);
    T remove(size_t index);
    size_t size() const;
  private:
    T* data_;              /* internal array     */
    size_t size_;        /* size of vector     */
    size_t numElements_; /* number of elements */
    size_t increase_;    /* increase           */

    /* initilize */
    void init(size_t size, size_t increase);

};

/*!
    \brief constructor

    constructor default size is 5

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HVector<T>::HVector() {

    init(5, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of vector

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HVector<T>::HVector(size_t size) {

    init(size, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of vector
    \param increase when resize this value used

    \author HigePon
    \date   create:2002/10/23 update:
*/
template <class T> HVector<T>::HVector(size_t size, size_t increase) {

    init(size, increase);
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

/*!
    \brief add element

    add element at the end of array

    \author HigePon
    \date   create:2002/10/23 update:
*/
template <class T> void HVector<T>::add(T element) {

    /* if array is full */
    if (size_ == numElements_) {

        /* resize array */
        size_ += increase_;
        T* temp = new T[size_];

        /* copy original to new array */
        for (int i = 0; i < numElements_; i++) {
            temp[i] = data_[i];
        }
        delete[] data_;
        data_ = temp;
    }

    /* add element */
    data_[numElements_] = element;
    numElements_++;
    return;
}

/*!
    \brief get

    get element at index

    \param index index of element to get

    \author HigePon
    \date   create:2002/10/23 update:
*/
template <class T> T HVector<T>::get(size_t index) const {

    /* check range */
    if (index < 0 || index >=numElements_) {
        _sys_printf("HVector<T>::get() out of bounds\n");
        return (T)NULL;
    }
    return data_[index];
}

/*!
    \brief operator[]

    get element at index

    \param index index of element to get

    \author HigePon
    \date   create:2002/10/23 update:
*/
template <class T> T HVector<T>::operator[](size_t index) {

    return (this->get(index));
}

/*!
    \brief size

    return size of vector

    \return size of vector

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> size_t HVector<T>::size() const {
    return numElements_;
}

/*!
    \brief remove element

    remove element at index

    \param index that removed

    \author HigePon
    \date   create:2002/10/23 update:
*/
template <class T> T HVector<T>::remove(size_t index) {

    /* check range */
    if (index < 0 || index >=numElements_) {

        /* do nothing */
        return (T)NULL;
    }

    /* save element to remove */
    T toRemove = data_[index];

    /* fix hole */
    for (int i = index; i < numElements_; i++) {
        data_[i] = data_[i + 1];
    }
    numElements_--;
    return toRemove;
}

/*!
    \brief initilize

    set size of vector & increase

    \author HigePon
    \date   create:2002/10/23 update:
*/
template <class T> void HVector<T>::init(size_t size, size_t increase) {

    /* number of elements */
    numElements_ = 0;

    /* set size and increase */
    size_     = size     > 0 ? size:5;
    increase_ = increase > 0 ? increase:5;

    /* create internal array */
    data_ = new T[size_];
    return;
}

#endif
