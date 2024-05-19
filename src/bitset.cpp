#include "bitset.h"

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset-view.h"

#include <algorithm>
#include <iostream>

bitset::bitset()
    : size_(0)
    , capacity_(0)
    , data_(nullptr) {}

bitset::bitset(std::size_t size, bool value)
    : size_(size)
    , capacity_((size + WORD_BITS - 1) / WORD_BITS)
    , data_(empty() ? nullptr : new word_type[capacity()]) {
  if (!empty()) {
    std::fill(data(), data() + capacity(), value ? -1 : 0);
  }
}

bitset::bitset(const bitset& other)
    : size_(other.size())
    , capacity_(other.capacity())
    , data_(new word_type[other.capacity()]) {
  std::copy_n(other.data(), other.capacity(), data());
}

bitset::bitset(std::string_view str)
    : size_(str.size())
    , capacity_((size() + WORD_BITS - 1) / WORD_BITS)
    , data_(empty() ? nullptr : new word_type[capacity()]{}) {
  for (std::size_t i = 0; i < str.size(); ++i) {
    (*this)[i] = (str[i] == '1');
  }
}

bitset::bitset(const bitset::const_view& other)
    : size_(other.size())
    , capacity_((size() + WORD_BITS - 1) / WORD_BITS)
    , data_(new word_type[capacity()]) {
  std::copy(other.begin(), other.end(), begin());
}

bitset::bitset(bitset::const_iterator first, bitset::const_iterator last)
    : size_(last - first)
    , capacity_((size() + WORD_BITS - 1) / WORD_BITS)
    , data_(new word_type[capacity()]) {
  std::copy(first, last, begin());
}

bitset::~bitset() {
  delete[] data_;
}

bitset& bitset::operator=(const bitset& other) & {
  if (other != *this) {
    bitset tmp(other);
    swap(tmp);
  }
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  bitset tmp(str);
  swap(tmp);
  return *this;
}

bitset& bitset::operator=(const bitset::const_view& other) & {
  bitset tmp(other);
  swap(tmp);
  return *this;
}

void bitset::swap(bitset& other) noexcept {
  std::swap(data_, other.data_);
  std::swap(size_, other.size_);
}

std::size_t bitset::size() const {
  return size_;
}

bool bitset::empty() const {
  return size() == 0;
}

bitset::iterator bitset::begin() {
  return {data_, 0};
}

bitset::const_iterator bitset::begin() const {
  return {data_, 0};
}

bitset::iterator bitset::end() {
  return begin() + size();
}

bitset::const_iterator bitset::end() const {
  return begin() + size();
}

bitset& bitset::operator&=(const bitset::const_view& other) & {
  for (std::size_t i = 0; i < size(); ++i) {
    (*this)[i] = (*this)[i] && other[i];
  }
  return *this;
}

bitset& bitset::operator|=(const bitset::const_view& other) & {
  for (std::size_t i = 0; i < size(); ++i) {
    (*this)[i] = (*this)[i] || other[i];
  }
  return *this;
}

bitset& bitset::operator^=(const bitset::const_view& other) & {
  for (std::size_t i = 0; i < size(); ++i) {
    (*this)[i] = (*this)[i] ^ other[i];
  }
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  if (size() <= count) {
    bitset tmp;
    swap(tmp);
    return *this;
  }
  bitset tmp(subview(0, size() - count));
  swap(tmp);
  return *this;
}

bitset& bitset::operator<<=(std::size_t count) & {
  bitset tmp(size() + count, false);
  // std::cout << (*this) << std::endl;
  // std::cout << count << ' ' << size() << ' ' << tmp.size() << ' ' << tmp.capacity() << std::endl;
  for (std::size_t i = 0; i < size(); ++i) {
    tmp[i] = (*this)[i];
  }
  swap(tmp);
  return *this;
}

void bitset::flip() & {
  for (std::size_t i = 0; i < capacity(); ++i) {
    data()[i] = ~data()[i];
  }
}

bitset& bitset::set() & {
  for (std::size_t i = 0; i < capacity(); ++i) {
    data()[i] = reference::ALL_BITS;
  }
  return *this;
}

bitset& bitset::reset() & {
  for (std::size_t i = 0; i < capacity(); ++i) {
    data()[i] = 0;
  }
  return *this;
}

bool bitset::all() const {
  for (auto it = begin(); it != end(); ++it) {
    if (!(*it)) {
      return false;
    }
  }
  return true;
}

bool bitset::any() const {
  for (auto it = begin(); it != end(); ++it) {
    if (*it) {
      return true;
    }
  }
  return false;
}

std::size_t bitset::count() const {
  std::size_t cnt = 0;
  for (auto it = begin(); it != end(); ++it) {
    if (*it) {
      ++cnt;
    }
  }
  return cnt;
}

bitset::reference bitset::operator[](std::size_t index) {
  return {data(), index};
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  return {data(), index};
}

bitset::operator const_view() const {
  return {begin(), end()};
}

bitset::operator view() {
  return {begin(), end()};
}

bitset::view bitset::subview(std::size_t offset, std::size_t count) {
  if (offset > size()) {
    return {};
  }
  if (count != npos && offset + count <= size()) {
    return {begin() + offset, begin() + offset + count};
  }
  return {begin() + offset, end()};
}

bitset::const_view bitset::subview(std::size_t offset, std::size_t count) const {
  if (offset > size()) {
    return {end(), end()};
  }
  if (count != npos && offset + count <= size()) {
    return {begin() + offset, begin() + offset + count};
  }
  return {begin() + offset, end()};
}

std::string to_string(const bitset& bs) {
  return to_string(bs.subview());
}

void swap(bitset& lhs, bitset& rhs) noexcept {
  lhs.swap(rhs);
}

std::ostream& operator<<(std::ostream& out, const bitset& bs) {
  std::string str = to_string(bs);
  out << str;
  return out;
}

bool operator==(const bitset& left, const bitset& right) {
  if (left.size() != right.size()) {
    return false;
  }
  for (std::size_t i = 0; i < left.size(); ++i) {
    if (left[i] != right[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const bitset& left, const bitset& right) {
  return !(left == right);
}

bitset operator&(const bitset& lhs, const bitset& rhs) {
  bitset result(lhs);
  return (result &= rhs);
}

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  return (result &= rhs);
}

bitset operator|(const bitset& lhs, const bitset& rhs) {
  bitset result(lhs);
  return (result |= rhs);
}

bitset operator|(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  return (result |= rhs);
}

bitset operator^(const bitset& lhs, const bitset& rhs) {
  bitset result(lhs);
  return (result ^= rhs);
}

bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  return (result ^= rhs);
}

bitset operator~(const bitset& bs) {
  bitset other(bs);
  other.flip();
  return other;
}

bitset operator~(const bitset::const_view& bs) {
  bitset other(bs);
  other.flip();
  return other;
}

bitset operator<<(const bitset& bs, std::size_t count) {
  bitset tmp(bs.size() + count, false);
  for (std::size_t i = 0; i < bs.size(); ++i) {
    tmp[i] = bs[i];
  }
  return tmp;
}

bitset operator>>(const bitset& bs, std::size_t count) {
  bitset tmp(bs.subview(0, bs.size() - count));
  return tmp;
}
