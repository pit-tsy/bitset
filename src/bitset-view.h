#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"

#include <functional>
#include <string>

template <typename T>
class bitset_view {
public:
  using value_type = bool;
  using reference = bitset_reference<T>;
  using const_reference = bitset_reference<const T>;
  using iterator = bitset_iterator<T>;
  using const_iterator = bitset_iterator<const T>;

  bitset_view() = default;

  bitset_view(bitset_iterator<T> begin, bitset_iterator<T> end)
      : begin_(begin)
      , end_(end) {}

  // bitset_view(const bitset_view& other)
  //     : begin_(other.begin())
  //     , end_(other.end()) {}

  operator bitset_view<const T>() const {
    return {begin_, end_};
  }

  // bitset_view& operator=(const bitset_view& other) {
  //   if (other != *this) {
  //     bitset_view tmp(other);
  //     swap(tmp);
  //   }
  //   return *this;
  // }

  std::size_t size() const {
    return end() - begin();
  }

  bool empty() const {
    return size() == 0;
  }

  bitset_reference<T> operator[](std::size_t index) const {
    return begin()[index];
  }

  bitset_iterator<T> begin() const {
    return begin_;
  }

  bitset_iterator<T> end() const {
    return end_;
  }

  void swap(bitset_view& other) noexcept {
    begin_.swap(other.begin_);
    end_.swap(other.end_);
  }

  bitset_view flip() const {
    for (auto b : *this) {
      b = !b;
    }
    return *this;
  }

  bitset_view set() const {
    for (auto it = begin(); it != end(); ++it) {
      *it = 1;
    }
    return *this;
  }

  bitset_view reset() const {
    for (auto it = begin(); it != end(); ++it) {
      *it = 0;
    }
    return *this;
  }

  bitset_view operator&=(const bitset_view<const T>& other) const {
    return applyOp(other, [](T a, T b) { return a & b; });
  }

  bitset_view operator|=(const bitset_view<const T>& other) const {
    return applyOp(other, [](T a, T b) { return a | b; });
  }

  bitset_view operator^=(const bitset_view<const T>& other) const {
    return applyOp(other, [](T a, T b) { return a ^ b; });
  }

  bool all() const {
    for (auto it = begin(); it != end(); ++it) {
      if (!(*it)) {
        return false;
      }
    }
    return true;
  }

  bool any() const {
    for (auto it = begin(); it != end(); ++it) {
      if (*it) {
        return true;
      }
    }
    return false;
  }

  std::size_t count() const {
    std::size_t cnt = 0;
    for (auto it = begin(); it != end(); ++it) {
      if (*it) {
        ++cnt;
      }
    }
    return cnt;
  }

  bitset_view<T> subview(std::size_t offset = 0, std::size_t count = -1) const {
    if (offset > size()) {
      return {};
    }
    if (count != -1 && offset + count <= size()) {
      return {begin() + offset, begin() + offset + count};
    }
    return {begin() + offset, end()};
  }

private:
  friend class bitset;

  bitset_view applyOp(const bitset_view<const T>& other, std::function<T(T a, T b)> op) const {
    for (std::size_t i = 0; i < size(); ++i) {
      (*this)[i] = op((*this)[i], other[i]);
    }
    return *this;
  }

  static constexpr std::size_t WORD_BITS = sizeof(T) * 8;

  bitset_iterator<T> begin_;
  bitset_iterator<T> end_;
};
