#ifndef _MONA_USERLIB_HLIST_
#define _MONA_USERLIB_HLIST_

#include <sys/types.h>
#include <sys/List.h>

/*----------------------------------------------------------------------
    HList Class
----------------------------------------------------------------------*/
template <class T> class HList : public List<T> {

  public:
    HList();
    HList(int size);
    HList(int size, int increase);
    virtual ~HList();

  public:
    void add(T element);
    T get(int index) const;
    T operator[](int index);
    T removeAt(int index);
    T remove(T element);
    int size() const;
    virtual bool isEmpty() const;
    bool hasElement(T element) const;

  private:
    T* data_;         /*! internal array     */
    int size_;        /*! size of liset      */
    int numElements_; /*! number of elements */
    int increase_;    /*! increase           */

    /* initilize */
    void init(int size, int increase);

};

/*!
    \brief constructor

    constructor default size is 5

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HList<T>::HList() {
    init(5, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of list

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> HList<T>::HList(int size) {

    init(size, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of list
    \param increase when resize this value used

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> HList<T>::HList(int size, int increase) {

    init(size, increase);
    return;
}

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> HList<T>::~HList() {

    /* release memory */
    delete[] data_;
    return;
}

/*!
    \brief isEmpty

    return is Empty or not

    \return true/false empty/has elements

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> bool HList<T>::isEmpty() const {

    return numElements_ == 0;
}

/*!
    \brief add element

    add element at the end of array

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> void HList<T>::add(T element) {

    /* if array is full */
    if (size_ == numElements_) {

        /* resize array */
        size_ += increase_;
        T* temp = new T[size_];

        /* optimize ? */
        int numElements = numElements_;

        /* copy original to new array */
        for (int i = 0; i < numElements; i++) {
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
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::get(int index) const {

    /* check range */
    if (index < 0 || index >=numElements_) {
        return (T)NULL;
    }
    return data_[index];
}

/*!
    \brief operator[]

    get element at index

    \param index index of element to get

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::operator[](int index) {

    return (this->get(index));
}

/*!
    \brief size

    return size of list

    \return size of list

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> int HList<T>::size() const {
    return numElements_;
}

/*!
    \brief remove element

    remove element at index

    \param index that removed

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::removeAt(int index) {

    /* check range */
    if (index < 0 || index >=numElements_) {

        /* do nothing */
        return (T)NULL;
    }

    /* save element to remove */
    T toRemove = data_[index];

    /* fix hole */
    int numElements = numElements_;
    for (int i = index; i < numElements - 1; i++) {
        data_[i] = data_[i + 1];
    }
    numElements_--;
    return toRemove;
}

/*!
    \brief remove element

    remove element

    \param element element to remove

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::remove(T element) {

    /* optimize */
    int size = this->size();

    for (int i = 0; i < size; i++) {

        /* element to remove found */
        if (data_[i] == element) {
            return (removeAt(i));
        }
    }

    return (T)NULL;
}

/*!
    \brief initilize

    set size of list & increase

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> void HList<T>::init(int size, int increase) {

    /* number of elements */
    numElements_ = 0;

    /* set size and increase */
    size_     = size     > 0 ? size : 5;
    increase_ = increase > 0 ? increase : 5;

    /* create internal array */
    data_ = new T[size_];
    return;
}

/*!
    \brief check list has the element

    \author HigePon
    \date   create:2003/12/21 update:
*/
template <class T> bool HList<T>::hasElement(T element) const {

    /* optimize? */
    int size = this->size();

    /* find element */
    for (int i = 0; i < size; i++) {

        if (data_[i] == element) {
            return true;
        }
    }

    /* not found */
    return false;
}

#endif
