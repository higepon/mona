#ifndef _MONA_USERLIB_HASHMAP_
#define _MONA_USERLIB_HASHMAP_

#include <sys/types.h>
#include <sys/BinaryTree.h>
#include <sys/Map.h>
/*----------------------------------------------------------------------
    For Hash
----------------------------------------------------------------------*/
inline char rol(char c) {

    asm volatile("rolb $1, %0  \n"
                 : /* no output */
                 : "m"(c)
                 );
    return c;
}

inline int xor2(int x, int y) {

    asm volatile ("mov %0   , %%eax  \n"
                 "xor %%eax, %1     \n"
                 : /* no output */
                 : "m"(y), "m"(x)
                 : "ax"
                 );

    return x;
}

/*----------------------------------------------------------------------
    HashMap
----------------------------------------------------------------------*/
template <class T> class HashMap : public Map<T> {

  public:
    HashMap(int hashsize);
    virtual ~HashMap();

  public:
    bool containsKey(char* key);
    T get(char* key);
    T put(char* key, T value);
    T remove(char* key);
    int size() const;

  private:
    HashMap() {}
    int hash(char* str);

  private:
    int hashsize_;
    BinaryTree<T>* tree_;
};

template <class T> HashMap<T>::HashMap(int hashsize) {
    hashsize_ = hashsize;
    tree_     = new BinaryTree<T>;
}

template <class T> HashMap<T>::~HashMap() {
    delete tree_;
}

template <class T> int HashMap<T>::hash(char* str) {

    int value = 0;

    /* get hash */
    for (; *str; str++) {
        value = rol(value);
        value = xor2(value, *str);
    }
    return value % hashsize_;
}

template <class T> int HashMap<T>::size() const {
    return tree_->size();
}

template <class T> bool HashMap<T>::containsKey(char* key) {
    return tree_->contains(hash(key));
}

template <class T> T HashMap<T>::get(char* key) {
    return tree_->get(hash(key));
}

template <class T> T HashMap<T>::put(char* key, T value) {
    return tree_->add(hash(key), value);
}

template <class T> T HashMap<T>::remove(char* key) {
    return tree_->remove(hash(key));
}
#endif
