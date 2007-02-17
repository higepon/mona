#ifndef __UTIL_HASH_MAP_H__
#define __UTIL_HASH_MAP_H__

#include "util/BinaryTree.h"
#include "util/Map.h"

namespace monash {
namespace util {

/*----------------------------------------------------------------------
    HashMap
----------------------------------------------------------------------*/
template <class T> class HashMap : public Map<T>
{
  public:
    HashMap();
    virtual ~HashMap();

  public:
    bool containsKey(const char* key);
    T get(const char* key);
    T put(const char* key, T value);
    T remove(const char* key);
    int size() const;

  private:
    int hash(const char* str);

  private:
    BinaryTree<T>* tree_;
};

template <class T> HashMap<T>::HashMap()
{
    tree_ = new BinaryTree<T>;
}

template <class T> HashMap<T>::~HashMap()
{
#ifndef MONASH_DONT_FREE_MEMORY
    delete tree_;
#endif
}

template <class T> int HashMap<T>::hash(const char* str)
{
    int length = strlen(str);
    int ret = 0;
    while (length-- > 0)
    {
        ret = (ret << 5) - ret + ((unsigned char)*str++);
    }
    return ret;
}

template <class T> int HashMap<T>::size() const
{
    return tree_->size();
}

template <class T> bool HashMap<T>::containsKey(const char* key)
{
    return tree_->contains(hash(key));
}

template <class T> T HashMap<T>::get(const char* key)
{
    return tree_->get(hash(key));
}

template <class T> T HashMap<T>::put(const char* key, T value)
{
    return tree_->add(hash(key), value);
}

template <class T> T HashMap<T>::remove(const char* key)
{
    return tree_->remove(hash(key));
}
};
};
#endif
