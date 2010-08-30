/*!
    \file  BitMap.h
    \brief class BitMap

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/03/30 update:$Date$
*/
#ifndef _MONA_BITMAP_
#define _MONA_BITMAP_

#include <sys/types.h>

#define temp_min(a, b) ((a < b) ? a : b)

// This class provides support for simple maps of bits.
class Bitmap {
 public:
  Bitmap() : map_(NULL), num_bits_(0), array_size_(0), alloc_(false) {}

  // This constructor will allocate on a uint32_t boundary. If |clear_bits| is
  // false, the bitmap bits will not be initialized.
  Bitmap(int num_bits, bool clear_bits)
     : num_bits_(num_bits), array_size_(RequiredArraySize(num_bits)),
       alloc_(true) {
    map_ = new uint32_t[array_size_];

    // Initialize all of the bits.
    if (clear_bits)
      Clear();
  }

// Returns the number of trailing zeros.
int FindLSBSetNonZero(uint32_t word) const {
  // Get the LSB, put it on the exponent of a 32 bit float and remove the
  // mantisa and the bias. This code requires IEEE 32 bit float compliance.
  float f = static_cast<float>(word & -static_cast<int>(word));

  // We use a union to go around strict-aliasing complains.
  union {
    float ieee_float;
    uint32_t as_uint;
  } x;

  x.ieee_float = f;
  return (x.as_uint >> 23) - 0x7f;
}

// Returns the index of the first bit set to |value| from |word|. This code
// assumes that we'll be able to find that bit.
int FindLSBNonEmpty(uint32_t word, bool value) const {
  // If we are looking for 0, negate |word| and look for 1.
  if (!value)
    word = ~word;

  return FindLSBSetNonZero(word);
}

bool FindNextBit(int* index, int limit, bool value) const {
  const int bit_index = *index;
  if (bit_index >= limit || limit <= 0)
    return false;

  // From now on limit != 0, since if it was we would have returned false.
  int word_index = bit_index >> kLogIntBits;
  uint32_t one_word = map_[word_index];

  // Simple optimization where we can immediately return true if the first
  // bit is set.  This helps for cases where many bits are set, and doesn't
  // hurt too much if not.
  if (Get(bit_index) == value)
    return true;

  const int first_bit_offset = bit_index & (kIntBits - 1);

  // First word is special - we need to mask off leading bits.
  uint32_t mask = 0xFFFFFFFF << first_bit_offset;
  if (value) {
    one_word &= mask;
  } else {
    one_word |= ~mask;
  }

  uint32_t empty_value = value ? 0 : 0xFFFFFFFF;

  // Loop through all but the last word.  Note that 'limit' is one
  // past the last bit we want to check, and we don't want to read
  // past the end of "words".  E.g. if num_bits_ == 32 only words[0] is
  // valid, so we want to avoid reading words[1] when limit == 32.
  const int last_word_index = (limit - 1) >> kLogIntBits;
  while (word_index < last_word_index) {
    if (one_word != empty_value) {
      *index = (word_index << kLogIntBits) + FindLSBNonEmpty(one_word, value);
      return true;
    }
    one_word = map_[++word_index];
  }

  // Last word is special - we may need to mask off trailing bits.  Note that
  // 'limit' is one past the last bit we want to check, and if limit is a
  // multiple of 32 we want to check all bits in this word.
  const int last_bit_offset = (limit - 1) & (kIntBits - 1);
  mask = 0xFFFFFFFE << last_bit_offset;
  if (value) {
    one_word &= ~mask;
  } else {
    one_word |= mask;
  }
  if (one_word != empty_value) {
    *index = (word_index << kLogIntBits) + FindLSBNonEmpty(one_word, value);
    return true;
  }
  return false;
}


void Set(int index, bool value) {
  const int i = index & (kIntBits - 1);
  const int j = index / kIntBits;
  if (value)
    map_[j] |= (1 << i);
  else
    map_[j] &= ~(1 << i);
}

bool Get(int index) const {
  const int i = index & (kIntBits-1);
  const int j = index / kIntBits;
  return map_[j] & (1 << i) ? true : false;
}

    int getBitsNumber() const
    {
        return num_bits_;
    }

    int countClear()
    {
        int count = 0;
        for (int i = 0; i < num_bits_; i++) {
            if (!marked(i)) count++;
        }

        return count;
    }




  void mark(int index)
  {
      Set(index, true);
  }

    bool marked(int index) const
    {
        return Get(index);
    }

    void clear(int index)
        {
            Set(index, false);
        }

  int find()
  {
      int index = 0;
      if (FindNextBit(&index, num_bits_, false)) {
          Set(index, true);
          return index;
      } else {
          return -1;
      }
  }

 int find(int num)
 {
        for (int i = 0; i < num_bits_; i++) {
            if (!marked(i)) {
                int size = 0;
                for (int j  = i; j < num_bits_; j++) {
                    if (marked(j)) {
                        break;
                    }
                    size++;
                    if (size == num) {
                        for (int k = i; k <= j; k++) {
                            mark(k);
                        }
                        return i;
                    }
                }
            }
        }
        return -1;
 }

  // Constructs a Bitmap with the actual storage provided by the caller. |map|
  // has to be valid until this object destruction. |num_bits| is the number of
  // bits in the bitmap, and |num_words| is the size of |map| in 32-bit words.
  Bitmap(uint32_t* map, int num_bits, int num_words)
     : map_(map), num_bits_(num_bits),
       // If size is larger than necessary, trim because array_size_ is used
       // as a bound by various methods.
       array_size_(temp_min(RequiredArraySize(num_bits), num_words)),
       alloc_(false) {}

  ~Bitmap() {
    if (alloc_)
      delete[] map_;
  }

  // Resizes the bitmap.
  // If |num_bits| < Size(), the extra bits will be discarded.
  // If |num_bits| > Size(), the extra bits will be filled with zeros if
  // |clear_bits| is true.
  // This object cannot be using memory provided during construction.
  void Resize(int num_bits, bool clear_bits);

  // Returns the number of bits in the bitmap.
  int Size() const { return num_bits_; }

  // Returns the number of 32-bit words in the bitmap.
  int ArraySize() const { return array_size_; }

  // Sets all the bits to true or false.
  void SetAll(bool value) {
    memset(map_, (value ? 0xFF : 0x00), array_size_ * sizeof(*map_));
  }

  // Clears all bits in the bitmap
  void Clear() { SetAll(false); }

  // Sets the value, gets the value or toggles the value of a given bit.
  void Toggle(int index);

  // Directly sets an element of the internal map. Requires |array_index| <
  // ArraySize();
  void SetMapElement(int array_index, uint32_t value);

  // Gets an entry of the internal map. Requires array_index <
  // ArraySize()
  uint32_t GetMapElement(int array_index) const;

  // Directly sets the whole internal map. |size| is the number of 32-bit words
  // to set from |map|. If  |size| > array_size(), it ignores the end of |map|.
  void SetMap(const uint32_t* map, int size);

  // Gets a pointer to the internal map.
  const uint32_t* GetMap() const { return map_; }

  // Sets a range of bits to |value|.
  void SetRange(int begin, int end, bool value);

  // Returns true if any bit between begin inclusive and end exclusive is set.
  // 0 <= |begin| <= |end| <= Size() is required.
  bool TestRange(int begin, int end, bool value) const;

  // Scans bits starting at bit *|index|, looking for a bit set to |value|. If
  // it finds that bit before reaching bit index |limit|, sets *|index| to the
  // bit index and returns true. Otherwise returns false.
  // Requires |limit| <= Size().
  //
  // Note that to use these methods in a loop you must increment the index
  // after each use, as in:
  //
  //  for (int index = 0 ; map.FindNextBit(&index, limit, value) ; ++index) {
  //    DoSomethingWith(index);
  //  }
//  bool FindNextBit(int* index, int limit, bool value) const;

  // Finds the first offset >= *|index| and < |limit| that has its bit set.
  // See FindNextBit() for more info.
  bool FindNextSetBitBeforeLimit(int* index, int limit) const {
    return FindNextBit(index, limit, true);
  }

  // Finds the first offset >= *|index| that has its bit set.
  // See FindNextBit() for more info.
  bool FindNextSetBit(int *index) const {
    return FindNextSetBitBeforeLimit(index, num_bits_);
  }

  // Scans bits starting at bit *|index|, looking for a bit set to |value|. If
  // it finds that bit before reaching bit index |limit|, sets *|index| to the
  // bit index and then counts the number of consecutive bits set to |value|
  // (before reaching |limit|), and returns that count. If no bit is found
  // returns 0. Requires |limit| <= Size().
  int FindBits(int* index, int limit, bool value) const;

  // Returns number of allocated words required for a bitmap of size |num_bits|.
  static int RequiredArraySize(int num_bits) {
    // Force at least one allocated word.
    if (num_bits <= kIntBits)
      return 1;

    return (num_bits + kIntBits - 1) >> kLogIntBits;
  }

 private:
  static const int kIntBits = sizeof(uint32_t) * 8;
  static const int kLogIntBits = 5;  // 2^5 == 32 bits per word.

  // Sets |len| bits from |start| to |value|. All the bits to be set should be
  // stored in the same word, and len < kIntBits.
  void SetWordBits(int start, int len, bool value);

  uint32_t* map_;           // The bitmap.
  int num_bits_;          // The upper bound of the bitmap.
  int array_size_;        // The physical size (in uint32_ts) of the bitmap.
  bool alloc_;            // Whether or not we allocated the memory.

};


// Originally from chromium/src/net/disk_cache/bitmap.cc
/*!
    BitMap claass
*/
/*!
    BitMap claass
*/
class BitMap {

public:
    BitMap(int number) : bitsNumber_(number)
    {
        pointerNum_ = (bitsNumber_ + POINTER_SIZE_BITS - 1) / POINTER_SIZE_BITS;
        map_ = new uintptr_t[pointerNum_];
        clearAll();
    }

    ~BitMap()
    {
        delete[] map_;
    }

    void mark(int index)
    {
        // this asserts slows down demand paging
//        ASSERT((index / POINTER_SIZE_BITS) < pointerNum_);
        map_[index / POINTER_SIZE_BITS] |= 1 << (index % POINTER_SIZE_BITS);
        return;
    }

    void clear(int index)
    {
//        ASSERT((index / POINTER_SIZE_BITS) < pointerNum_);
        map_[index / POINTER_SIZE_BITS] &= ~(1 << (index % POINTER_SIZE_BITS));
        return;
    }

    int find()
    {
        for (int i = 0; i < bitsNumber_; i++) {
            if (!marked(i)) {
                mark(i);
                return i;
            }
        }
        return -1;
    }

    int find(int num)
    {
        for (int i = 0; i < bitsNumber_; i++) {
            if (!marked(i)) {
                int size = 0;
                for (int j  = i; j < bitsNumber_; j++) {
                    if (marked(j)) {
                        break;
                    }
                    size++;
                    if (size == num) {
                        for (int k = i; k <= j; k++) {
                            mark(k);
                        }
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    int countClear()
    {
        int count = 0;
        for (int i = 0; i < bitsNumber_; i++) {
            if (!marked(i)) count++;
        }

        return count;
    }

    bool marked(int index)
    {
        ASSERT((index / POINTER_SIZE_BITS) < pointerNum_);
        return(map_[index / POINTER_SIZE_BITS] & (1 << (index % POINTER_SIZE_BITS)));
    }

    int getBitsNumber() const
    {
        return bitsNumber_;
    }

private:
    enum
    {
        POINTER_SIZE_BITS = sizeof(uintptr_t) * 8
    };

    void clearAll()
    {
        memset(map_, 0, sizeof(uintptr_t) * pointerNum_);
    }

    uintptr_t* map_;
    int bitsNumber_;
    int pointerNum_;
};


#endif
