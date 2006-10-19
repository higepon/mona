#ifndef _MONA_USERLIB_BINARY_TREE_
#define _MONA_USERLIB_BINARY_TREE_

#include <sys/types.h>

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
        return add(tree->left, key, element);

    /* add to right */
    } else {
        return add(tree->right, key, element);
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

#endif
