#ifndef _HIGEPOS_VECTOR_
#define _HIGEPOS_VECTOR_

template <class T> class HVector {

  public:
    HVector();
    HVector(unsigned long size);
    ~HVector();
    void add(T element);
    T get(unsigned long index);
    T remove(unsigned long index);
    unsigned long size();
};
template <class T> HVector<T>::HVector() {
    return;
}

template <class T> HVector<T>::HVector(unsigned long size) {
    return;
}

template <class T> HVector<T>::~HVector() {
    return;
}

template <class T> void HVector<T>::add(T element) {
    return;
}

template <class T> T HVector<T>::get(unsigned long index) {
    return (T)0;
}


template <class T> unsigned long HVector<T>::size() {
}

template <class T> T HVector<T>::remove(unsigned long index) {


}

#endif
