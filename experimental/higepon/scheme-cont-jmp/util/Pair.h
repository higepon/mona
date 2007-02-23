#ifndef __UTIL_PAIR_H__
#define __UTIL_PAIR_H__

namespace util {

/*----------------------------------------------------------------------
    Pair
----------------------------------------------------------------------*/
template <class T, class S> struct Pair
{
  public:
    Pair(const T& t, const S& s) : first(t), second(s) {}
    Pair() : first(), second() {}
    ~Pair() {}
    T first;
    S second;
};

};
#endif // __UTIL_PAIR_H__
