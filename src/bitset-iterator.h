#pragma once

#include "bitset-reference.h"

#include <cmath>
#include <compare>
#include <iterator>

template <typename T>
class bitset_iterator {
public:
  using value_type = bool;
  using difference_type = std::ptrdiff_t;
  using reference = bitset_reference<T>;
  using pointer = void;
  using iterator_category = std::random_access_iterator_tag;

  bitset_iterator() = default;

  operator bitset_iterator<const T>() const {
    return {word_ptr_, bit_index_};
  }

  void swap(bitset_iterator& other) noexcept {
    std::swap(word_ptr_, other.word_ptr_);
    std::swap(bit_index_, other.bit_index_);
  }

  reference operator*() const {
    return reference(word_ptr_, bit_index_);
  }

  reference operator[](difference_type index) const {
    return reference(word_ptr_, bit_index_ + index);
  }

  bitset_iterator& operator++() {
    if (bit_index_ + 1 == WORD_BITS) {
      bit_index_ = 0;
      ++word_ptr_;
    } else {
      ++bit_index_;
    }
    return *this;
  }

  bitset_iterator operator++(int) {
    bitset_iterator copy = *this;
    ++*this;
    return copy;
  }

  bitset_iterator& operator--() {
    if (bit_index_ == 0) {
      bit_index_ = WORD_BITS - 1;
      --word_ptr_;
    } else {
      --bit_index_;
    }
    return *this;
  }

  bitset_iterator operator--(int) {
    bitset_iterator copy = *this;
    --*this;
    return copy;
  }

  bitset_iterator& operator+=(const difference_type n) {
    bitset_iterator<T> tmp(word_ptr_, bit_index_ + n);
    swap(tmp);
    return *this;
  }

  bitset_iterator& operator-=(const difference_type n) {
    bitset_iterator tmp(word_ptr_, bit_index_ - n);
    swap(tmp);
    return *this;
  }

  friend bitset_iterator operator+(const difference_type lhs, const bitset_iterator& rhs) {
    bitset_iterator result = rhs;
    return (result += lhs);
  }

  friend bitset_iterator operator+(const bitset_iterator& lhs, const difference_type rhs) {
    return rhs + lhs;
  }

  friend bitset_iterator operator-(const difference_type lhs, const bitset_iterator& rhs) {
    return -(rhs - lhs);
  }

  friend bitset_iterator operator-(const bitset_iterator& lhs, const difference_type rhs) {
    bitset_iterator result = lhs;
    return (result -= rhs);
  }

  friend difference_type operator-(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return (lhs.word_ptr_ - rhs.word_ptr_) * WORD_BITS + (lhs.bit_index_ - rhs.bit_index_);
  }

  friend bool operator==(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return lhs.word_ptr_ == rhs.word_ptr_ && lhs.bit_index_ == rhs.bit_index_;
  }

  friend std::strong_ordering operator<=>(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    if (lhs.word_ptr_ == rhs.word_ptr_) {
      return lhs.bit_index_ <=> rhs.bit_index_;
    } else {
      return lhs.word_ptr_ <=> rhs.word_ptr_;
    }
  }

private:
  bitset_iterator(T* data_, difference_type bit_index)
      : word_ptr_(calc_word(data_, bit_index))
      , bit_index_(bit_index % WORD_BITS) {}

  T* calc_word(T* cur_word, difference_type bit_index) const {
    T* result = cur_word + (bit_index / static_cast<difference_type>(WORD_BITS));
    if (bit_index < 0 && bit_index % WORD_BITS != 0) {
      --result;
    }
    return result;
  }

  friend class bitset;

  template <typename K>
  friend class bitset_iterator;

  template <typename K>
  friend class bitset_view;

private:
  static constexpr std::size_t WORD_BITS = sizeof(T) * 8;

  T* word_ptr_;
  difference_type bit_index_;
};

template <typename T>
void swap(bitset_iterator<T>& lhs, bitset_iterator<T>& rhs) noexcept {
  lhs.swap(rhs);
}
