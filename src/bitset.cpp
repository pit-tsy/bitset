#include "bitset.h"

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset-view.h"

#include <algorithm>

bitset::bitset()
    : size_(0)
    , capacity_(0)
    , data_(nullptr) {}

bitset::bitset(std::size_t size)
    : size_(size)
    , capacity_((size + bitset_common::WORD_BITS - 1) / bitset_common::WORD_BITS)
    , data_(empty() ? nullptr : new word_type[capacity()]) {}

bitset::bitset(std::size_t size, bool value)
    : bitset(size) {
  if (!empty()) {
    std::fill(data(), data() + capacity(), value ? -1 : 0);
  }
}

bitset::bitset(const bitset& other)
    : bitset(other.size()) {
  std::copy_n(other.data(), other.capacity(), data());
}

bitset::bitset(std::string_view str)
    : bitset(str.size()) {
  for (std::size_t i = 0; i < str.size(); ++i) {
    (*this)[i] = (str[i] == '1');
  }
}

bitset::bitset(const bitset::const_view& other)
    : bitset(other.size()) {
  std::copy(other.begin(), other.end(), begin());
}

bitset::bitset(bitset::const_iterator first, bitset::const_iterator last)
    : bitset(last - first) {
  subview().assign(bitset_view(first, last));
}

bitset::~bitset() {
  delete[] data_;
}

bitset& bitset::operator=(const bitset& other) & {
  if (&other != this) {
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
  std::swap(capacity_, other.capacity_);
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
  subview() &= other.subview();
  return *this;
}

bitset& bitset::operator|=(const bitset::const_view& other) & {
  subview() |= other.subview();
  return *this;
}

bitset& bitset::operator^=(const bitset::const_view& other) & {
  subview() ^= other.subview();
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
  tmp.subview(0, size()).assign(subview());
  swap(tmp);
  return *this;
}

void bitset::flip() & {
  subview().flip();
}

bitset& bitset::set() & {
  subview().set();
  return *this;
}

bitset& bitset::reset() & {
  subview().reset();
  return *this;
}

bool bitset::all() const {
  return subview().all();
}

bool bitset::any() const {
  return subview().any();
}

std::size_t bitset::count() const {
  return subview().count();
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
    return {end(), end()};
  }
  if (count != npos && count <= size() - offset) {
    return {begin() + offset, begin() + offset + count};
  }
  return {begin() + offset, end()};
}

bitset::const_view bitset::subview(std::size_t offset, std::size_t count) const {
  if (offset > size()) {
    return {end(), end()};
  }
  if (count != npos && count <= size() - offset) {
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

void swap(bitset::reference& lhs, bitset::reference& rhs) noexcept {
  lhs.swap(rhs);
}

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  return (result &= rhs);
}

bitset operator|(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  return (result |= rhs);
}

bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  return (result ^= rhs);
}

bitset operator~(const bitset::const_view& bs) {
  bitset other(bs);
  other.flip();
  return other;
}

bitset operator<<(const bitset::const_view& bs, std::size_t count) {
  bitset tmp(bs.size() + count, false);
  tmp.subview(0, bs.size()).assign(bs);
  return tmp;
}

bitset operator>>(const bitset::const_view& bs, std::size_t count) {
  bitset tmp(bs.subview(0, bs.size() - count));
  return tmp;
}

bool operator==(const bitset::const_view& left, const bitset::const_view& right) {
  if (left.size() != right.size()) {
    return false;
  }
  std::size_t n = 0;
  for (; n + 1 < left.words_number(); ++n) {
    if (left.get_word(n) != right.get_word(n)) {
      return false;
    }
  }
  std::size_t word_size = left.size() - bitset_common::WORD_BITS * n;
  return left.get_word(n, word_size) == right.get_word(n, word_size);
}

bool operator!=(const bitset::const_view& left, const bitset::const_view& right) {
  return !(left == right);
}

std::string to_string(const bitset::const_view& bs) {
  std::string result;
  for (auto b : bs) {
    result += (b ? '1' : '0');
  }
  return result;
}

std::ostream& operator<<(std::ostream& out, const bitset::const_view& bs) {
  for (auto b : bs) {
    out << (b ? '1' : '0');
  }
  return out;
}
