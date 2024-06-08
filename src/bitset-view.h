#pragma once

#include "bitset-common.h"
#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset.h"

#include <bit>
#include <cmath>

template <typename T>
class bitset_view {
public:
  using value_type = bool;
  using word_type = T;
  using reference = bitset_reference<word_type>;
  using const_reference = bitset_reference<const word_type>;
  using iterator = bitset_iterator<word_type>;
  using const_iterator = bitset_iterator<const word_type>;
  using view = bitset_view<word_type>;
  using const_view = bitset_view<const word_type>;

  bitset_view() = default;

  bitset_view(bitset_iterator<T> begin, bitset_iterator<T> end)
      : begin_(begin)
      , end_(end) {}

  operator bitset_view<const T>() const {
    return {begin_, end_};
  }

  std::size_t size() const {
    return end() - begin();
  }

  bool empty() const {
    return size() == 0;
  }

  reference operator[](std::size_t index) const {
    return begin()[index];
  }

  iterator begin() const {
    return begin_;
  }

  iterator end() const {
    return end_;
  }

  void swap(bitset_view& other) noexcept {
    begin_.swap(other.begin_);
    end_.swap(other.end_);
  }

  template <bitset_common::NonConst U = T>
  view flip() const {
    return applyUnaryOp([](T x) { return ~x; });
  }

  template <bitset_common::NonConst U = T>
  view set() const {
    return applyUnaryOp([](T) { return bitset_common::ALL_BITS; });
  }

  template <bitset_common::NonConst U = T>
  view reset() const {
    return applyUnaryOp([](T) { return bitset_common::ZERO; });
  }

  template <bitset_common::NonConst U = T>
  view operator&=(const bitset_view<const T>& other) const {
    return applyBinaryOp(other, [](T a, T b) { return a & b; });
  }

  template <bitset_common::NonConst U = T>
  view operator|=(const bitset_view<const T>& other) const {
    return applyBinaryOp(other, [](T a, T b) { return a | b; });
  }

  template <bitset_common::NonConst U = T>
  view operator^=(const bitset_view<const T>& other) const {
    return applyBinaryOp(other, [](T a, T b) { return a ^ b; });
  }

  bool all() const {
    std::size_t n = 0;
    for (; n + 1 < words_number(); ++n) {
      if (get_word(n) != bitset_common::ALL_BITS) {
        return false;
      }
    }
    std::size_t word_size = size() - bitset_common::WORD_BITS * n;
    return word_size == 0 ||
           get_word(n, word_size) == (bitset_common::ALL_BITS >> (bitset_common::WORD_BITS - word_size));
  }

  bool any() const {
    std::size_t n = 0;
    for (; n + 1 < words_number(); ++n) {
      if (get_word(n) != bitset_common::ZERO) {
        return true;
      }
    }
    std::size_t word_size = size() - bitset_common::WORD_BITS * n;
    return get_word(n, word_size) != bitset_common::ZERO;
  }

  template <bitset_common::NonConst U = T>
  view assign(const bitset_view<const T>& other) {
    return applyBinaryOp(other, [](T, T b) { return b; });
  }

  std::size_t count() const {
    std::size_t result = 0;
    std::size_t n = 0;
    for (; n + 1 < words_number(); ++n) {
      result += std::popcount(get_word(n));
    }
    std::size_t word_size = size() - bitset_common::WORD_BITS * n;
    result += std::popcount(get_word(n, word_size));
    return result;
  }

  view subview(std::size_t offset = 0, std::size_t count = -1) const {
    if (offset > size()) {
      return {end(), end()};
    }
    if (count != -1 && count <= size() - offset) {
      return {begin() + offset, begin() + offset + count};
    }
    return {begin() + offset, end()};
  }

  friend bool operator==(const bitset_view<const word_type>& left, const bitset_view<const word_type>& right);

private:
  friend class bitset;

  template <typename K>
  friend class bitset_view;

  word_type get_word(std::size_t word_num = 0, std::size_t bits = bitset_common::WORD_BITS) const {
    return bits == 0 ? 0 : begin().get_word(word_num, bits);
  }

  void set_word(word_type value, std::size_t word_num = 0, std::size_t bits = bitset_common::WORD_BITS) const {
    if (bits > 0) {
      begin().set_word(value, word_num, bits);
    }
  }

  std::size_t words_number() const {
    return (size() + bitset_common::WORD_BITS - 1) / bitset_common::WORD_BITS;
  }

  template <bitset_common::NonConst U = T, typename Func>
  view applyBinaryOp(const bitset_view<const T>& other, Func op) const {
    std::size_t n = 0;
    for (; n + 1 < words_number(); ++n) {
      auto word1 = get_word(n);
      auto word2 = other.get_word(n);
      set_word(op(word1, word2), n);
    }
    std::size_t word_size = size() - bitset_common::WORD_BITS * n;
    auto last_word1 = get_word(n, word_size);
    auto last_word2 = other.get_word(n, word_size);
    set_word(op(last_word1, last_word2), n, word_size);
    return *this;
  }

  template <bitset_common::NonConst U = T, typename Func>
  view applyUnaryOp(Func op) const {
    std::size_t n = 0;
    for (; n + 1 < words_number(); ++n) {
      auto word1 = get_word(n);
      set_word(op(word1), n);
    }
    std::size_t word_size = size() - bitset_common::WORD_BITS * n;
    auto last_word = get_word(n, word_size);
    set_word(
        op(last_word) << (bitset_common::WORD_BITS - word_size) >> (bitset_common::WORD_BITS - word_size),
        n,
        word_size
    );

    return *this;
  }

  iterator begin_;
  iterator end_;
};
