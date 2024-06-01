#pragma once

#include "bitset-common.h"

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
    *word_ = (*word_ & (bitset_common::ALL_BITS ^ (bitset_common::ONE << bit_index_))) | (T(b) << bit_index_);
    return {word_, bit_index_};
  }

  template <bitset_common::NonConst U = T>
  bitset_reference operator&=(bool b) const {
    return operator=(bool(*this) && b);
  }

  template <bitset_common::NonConst U = T>
  bitset_reference operator|=(bool b) const {
    return operator=(bool(*this) || b);
  }

  template <bitset_common::NonConst U = T>
  bitset_reference operator^=(bool b) const {
    return operator=(bool(*this) ^ b);
    ;
  }

  operator bitset_reference<const T>() const {
    return {word_, bit_index_};
  }

  bitset_reference operator[](std::ptrdiff_t index) const {
    return bitset_reference(word_, bit_index_ + index);
  }

  template <bitset_common::NonConst U = T>
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
      : word_(data_ + bit_index / bitset_common::WORD_BITS)
      , bit_index_(bit_index % bitset_common::WORD_BITS) {}

  friend bitset;

  template <typename K>
  friend class bitset_iterator;

  template <typename K>
  friend class bitset_reference;

private:
  T* word_;
  std::size_t bit_index_;
};
