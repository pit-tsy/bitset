#pragma once

#include <cstddef>
#include <iostream>
#include <utility>

class bitset;

template <typename T>
class bitset_iterator;

template <typename T>
class bitset_reference {
public:
  bitset_reference operator=(bool b) const {
    *word_ = (*word_ & (ALL_BITS ^ (T(1) << bit_index_))) | (T(b) << bit_index_);
    return {word_, bit_index_};
  }

  // bitset_reference& operator=(const bitset_reference& other) {
  //   *word_ = (*word_ & (ALL_BITS ^ (T(1) << bit_index_))) | (T(other) << bit_index_);
  //   return *this;
  // }

  bitset_reference operator&=(bool b) const {
    return operator=(bool(*this) && b);
  }

  bitset_reference operator|=(bool b) const {
    return operator=(bool(*this) || b);
  }

  bitset_reference operator^=(bool b) const {
    return operator=(bool(*this) ^ b);
    ;
  }

  operator bitset_reference<const T>() const {
    return {word_, bit_index_};
  }

  bitset_reference operator[](std::size_t index) const {
    return bitset_reference(word_, bit_index_ + index);
  }

  void flip() const {
    *word_ ^= T(1) << bit_index_;
  }

  operator bool() const {
    return (*word_ >> bit_index_) & 1;
  }

  void swap(bitset_reference& other) noexcept {
    std::swap(word_, other.word_);
    std::swap(word_, other.word_);
  }

private:
  bitset_reference(T* data_, std::size_t bit_index)
      : word_(data_ + bit_index / WORD_BITS)
      , bit_index_((bit_index % WORD_BITS + WORD_BITS) % WORD_BITS) {}

  friend bitset;

  template <typename K>
  friend class bitset_iterator;

  template <typename K>
  friend class bitset_reference;

private:
  static constexpr T ALL_BITS = -1;
  static constexpr std::size_t WORD_BITS = sizeof(T) * 8;

  T* word_;
  std::size_t bit_index_;
};
