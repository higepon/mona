#ifndef _MONA_COLLECTION_
#define _MONA_COLLECTION_

#include<types.h>

/*!
    \file  collection.h
    \brief collection utilities

    !!! in first use, you should debug this class. !!!

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/12/07 update:$Date$
*/

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
    Bynary Tree
----------------------------------------------------------------------*/
#define NO_DATA (Node*)0
template <class T> class BinaryTree {

  public:
    BinaryTree();
    virtual ~BinaryTree();

  public:
    int  size() const;
    T get(const int key) const;
    bool contains(const int key) const;
    T add(const int key, const T element);
    T remove(const int key);

  private:
    struct Node {
        int key;
        T element;
        Node* left;
        Node* right;
    };

    Node* root_;
    int numberOfElements_;

  private:
    T add(Node*& tree, const int key, const T element);
    bool contains(const Node* tree, const int key) const;
    void clear();
    void clear(Node*& tree);
    T get(const Node* tree, const int key) const;
};

template <class T> BinaryTree<T>::BinaryTree() : root_(NO_DATA), numberOfElements_(0) {
}

template <class T> BinaryTree<T>::~BinaryTree() {
    clear();
}

template <class T> int BinaryTree<T>::size() const {
    return numberOfElements_;
}

template <class T> T BinaryTree<T>::get(const int key) const {
    return get(root_, key);
}

template <class T> void BinaryTree<T>::clear() {
    clear(root_);
}

template <class T> T BinaryTree<T>::add(const int key, const T element) {

    return add(root_, key, element);
}

template <class T> bool BinaryTree<T>::contains(const int key) const {
    return contains(root_, key);
}

template <class T> T BinaryTree<T>::get(const Node* tree, const int key) const {

    if (tree == NO_DATA) {
        return (T)0;
    }

    if (key == tree->key) {
        return tree->element;
    } else if (key < tree->key) {
        return get(tree->left, key);
    } else {
        return get(tree->right, key);
    }
}

template <class T> void BinaryTree<T>::clear(Node*& tree) {

    if (tree != NO_DATA) {
        clear(tree->left);
        clear(tree->right);
        delete tree;
        tree = NO_DATA;
    }
}

template <class T> T BinaryTree<T>::add(Node*& tree, const int key, const T element) {

    /* add */
    if (tree == NO_DATA) {
        tree = new Node;

        if (tree == NULL) {

            /* not implemented */
            for (;;);
        }

        tree->left    = NO_DATA;
        tree->right   = NO_DATA;
        tree->key     = key;
        tree->element = element;
        numberOfElements_++;
        return element;

    /* data overwrite */
    } else if (key == tree->key) {
        T previousElement = tree->element;
        tree->element = element;
        return previousElement;

    /* add to left */
    } else if (key < tree->key) {
        add(tree->left, key, element);

    /* add to right */
    } else {
        add(tree->right, key, element);
    }
}

template <class T> bool BinaryTree<T>::contains(const Node* tree, const int key) const {

    if (tree == NO_DATA) {
        return false;
    } else if (key == tree->key) {
        return true;
    } else if (key < tree->key) {
        return contains(tree->left, key);
    } else {
        return contains(tree->right, key);
    }
}

template <class T> T BinaryTree<T>::remove(const int key) {

    T result;
    Node** tree = &root_;
    Node* next;
    Node* temp;
    Node** leftMax;

    for (;;) {

        /* not found */
        if (*tree == NO_DATA) {
            return (T)NULL;
        }

        /* found */
        if (key == (*tree)->key) {
            result = (*tree)->element;
            break;
        }

        /* search next */
        if (key < (*tree)->key) {
            tree = &((*tree)->left);
        } else {
            tree = &((*tree)->right);
        }
    }

    if ((*tree)->left == NO_DATA) {
        next = (*tree)->right;
    } else {
        leftMax = &((*tree)->left);

        while ((*leftMax)->right != NO_DATA) {
            leftMax = &((*leftMax)->right);
        }
        next = *leftMax;
        *leftMax    = (*leftMax)->left;
        next->left  = (*tree)->left;
        next->right = (*tree)->right;
    }

    temp = *tree;
    *tree = next;
    delete temp;
    numberOfElements_--;
    return result;
};

/*----------------------------------------------------------------------
    Map Interface
----------------------------------------------------------------------*/
template <class T> class Map {

  public:
    virtual bool containsKey(char* key) = 0;
    virtual T get(char* key)            = 0;
    virtual T put(char* key, T value)   = 0;
    virtual T remove(char* key)         = 0;
    virtual int size()          const = 0;
};

/*----------------------------------------------------------------------
    List Interface
----------------------------------------------------------------------*/
template <class T> class List {

  public:
    virtual void add(T element)              = 0;
    virtual T removeAt(size_t index)         = 0;
    virtual T remove(T element)              = 0;
    virtual T get(size_t index) const        = 0;
    virtual bool isEmpty() const             = 0;
    virtual size_t size() const              = 0;
    virtual bool hasElement(T element) const = 0;
};

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

/*----------------------------------------------------------------------
    HList Class
----------------------------------------------------------------------*/
template <class T> class HList : public List<T> {

  public:
    HList();
    HList(size_t size);
    HList(size_t size, size_t increase);
    virtual ~HList();

  public:
    void add(T element);
    T get(size_t index) const;
    T operator[](size_t index);
    T removeAt(size_t index);
    T remove(T element);
    size_t size() const;
    virtual bool isEmpty() const;
    bool hasElement(T element) const;

  private:
    T* data_;            /*! internal array     */
    size_t size_;        /*! size of liset      */
    size_t numElements_; /*! number of elements */
    size_t increase_;    /*! increase           */

    /* initilize */
    void init(size_t size, size_t increase);

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
template <class T> HList<T>::HList(size_t size) {

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
template <class T> HList<T>::HList(size_t size, size_t increase) {

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

        /* copy original to new array */
        for (size_t i = 0; i < numElements_; i++) {
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
template <class T> T HList<T>::get(size_t index) const {

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
template <class T> T HList<T>::operator[](size_t index) {

    return (this->get(index));
}

/*!
    \brief size

    return size of list

    \return size of list

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> size_t HList<T>::size() const {
    return numElements_;
}

/*!
    \brief remove element

    remove element at index

    \param index that removed

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::removeAt(size_t index) {

    /* check range */
    if (index < 0 || index >=numElements_) {

        /* do nothing */
        return (T)NULL;
    }

    /* save element to remove */
    T toRemove = data_[index];

    /* fix hole */
    for (size_t i = index; i < numElements_ - 1; i++) {
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

    for (int i = 0; i < size(); i++) {

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
template <class T> void HList<T>::init(size_t size, size_t increase) {

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

    /* find element */
    for (int i = 0; i < size(); i++) {

        if (data_[i] == element) {
            return true;
        }
    }

    /* not found */
    return false;
}

#endif
