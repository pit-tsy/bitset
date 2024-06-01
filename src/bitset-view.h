#pragma once

#include "bitset-common.h"
#include "bitset-iterator.h"
#include "bitset-reference.h"

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
    for (auto it = begin(); it != end();) {
      std::size_t bits = std::min(bitset_common::WORD_BITS, static_cast<std::size_t>(end() - it));
      if (it.get_word(bits) != bitset_common::ALL_BITS >> (bitset_common::WORD_BITS - bits)) {
        return false;
      }
      it += bits;
    }
    return true;
  }

  bool any() const {
    for (auto it = begin(); it != end();) {
      std::size_t bits = std::min(bitset_common::WORD_BITS, static_cast<std::size_t>(end() - it));
      if (it.get_word(bits)) {
        return true;
      }
      it += bits;
    }
    return false;
  }

  template <bitset_common::NonConst U = T>
  view assign(const bitset_view<const T>& other) {
    auto it = begin();
    auto other_it = other.begin();
    while (it != end()) {
      std::size_t bits = std::min(bitset_common::WORD_BITS, static_cast<std::size_t>(end() - it));
      it.set_word(other_it.get_word(bits), bits);
      it += bits;
      other_it += bits;
    }
    return *this;
  }

  std::size_t count() const {
    std::size_t cnt = 0;
    for (auto it = begin(); it != end();) {
      std::size_t bits = std::min(bitset_common::WORD_BITS, static_cast<std::size_t>(end() - it));
      cnt += std::popcount(it.get_word(bits));
      it += bits;
    }
    return cnt;
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

private:
  friend class bitset;

  template <bitset_common::NonConst U = T, typename Func>
  view applyBinaryOp(const bitset_view<const T>& other, Func op) const {
    auto it = begin();
    auto other_it = other.begin();
    while (it != end()) {
      std::size_t bits = std::min(bitset_common::WORD_BITS, static_cast<std::size_t>(end() - it));
      auto word1 = it.get_word(bits);
      auto word2 = other_it.get_word(bits);
      it.set_word(op(word1, word2), bits);
      it += bits;
      other_it += bits;
    }
    return *this;
  }

  template <bitset_common::NonConst U = T, typename Func>
  view applyUnaryOp(Func op) const {
    for (auto it = begin(); it != end();) {
      std::size_t bits = std::min(bitset_common::WORD_BITS, static_cast<std::size_t>(end() - it));
      it.set_word(
          op(it.get_word(bits)) << (bitset_common::WORD_BITS - bits) >> (bitset_common::WORD_BITS - bits),
          bits
      );
      it += bits;
    }
    return *this;
  }

  iterator begin_;
  iterator end_;
};
