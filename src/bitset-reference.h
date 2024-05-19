#pragma once

// #include "bitset.h"
#include <cstddef>
// #include <iostream>

class bitset;

template <typename T>
class bitset_iterator;

template <typename T>
class bitset_reference {
public:
  bitset_reference& operator=(bool b) {
    *word_ = *word_ & (ALL_BITS ^ (T(1) << bit_index_)) | (T(b) << bit_index_);
    return *this;
  }

  bitset_reference& operator=(const bitset_reference& other) {
    *word_ = *word_ & (ALL_BITS ^ (T(1) << bit_index_)) | (T(other) << bit_index_);
    return *this;
  }

  bitset_reference operator[](std::size_t index) & {
    return bitset_reference(word_, bit_index_ + index);
  }

  void flip() {
    *word_ ^= T(1) << bit_index_;
  }

  operator bool() const {
    return (*word_ >> bit_index_) & 1;
  }

private:
  bitset_reference(T* data_, std::size_t bit_index)
      : word_(data_ + bit_index / WORD_BITS)
      , bit_index_(bit_index % WORD_BITS) {}

  friend bitset;
  friend bitset_iterator<T>;

private:
  static constexpr T ALL_BITS = -1;
  static constexpr std::size_t WORD_BITS = sizeof(T) * 8;

  T* word_;
  std::size_t bit_index_;
};
