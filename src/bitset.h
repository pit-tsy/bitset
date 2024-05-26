#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset-view.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

class bitset {
public:
  using value_type = bool;
  using word_type = uint64_t;
  using reference = bitset_reference<word_type>;
  using const_reference = bitset_reference<const word_type>;
  using iterator = bitset_iterator<word_type>;
  using const_iterator = bitset_iterator<const word_type>;
  using view = bitset_view<word_type>;
  using const_view = bitset_view<const word_type>;

  static constexpr std::size_t npos = -1;

  bitset();
  bitset(std::size_t size, bool value);
  bitset(const bitset& other);
  explicit bitset(std::string_view str);
  explicit bitset(const const_view& other);
  bitset(const_iterator first, const_iterator last);

  bitset& operator=(const bitset& other) &;
  bitset& operator=(std::string_view str) &;
  bitset& operator=(const const_view& other) &;

  ~bitset();

  void swap(bitset& other) noexcept;

  std::size_t size() const;
  bool empty() const;

  reference operator[](std::size_t index);
  const_reference operator[](std::size_t index) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bitset& operator&=(const const_view& other) &;
  bitset& operator|=(const const_view& other) &;
  bitset& operator^=(const const_view& other) &;
  bitset& operator<<=(std::size_t count) &;
  bitset& operator>>=(std::size_t count) &;
  void flip() &;

  bitset& set() &;
  bitset& reset() &;

  bool all() const;
  bool any() const;
  std::size_t count() const;

  operator const_view() const;
  operator view();

  view subview(std::size_t offset = 0, std::size_t count = npos);
  const_view subview(std::size_t offset = 0, std::size_t count = npos) const;

private:
  std::size_t capacity() const {
    return capacity_;
  }

  word_type* data() const {
    return data_;
  }

  static constexpr std::size_t WORD_BITS = sizeof(word_type) * 8;

private:
  std::size_t size_;
  std::size_t capacity_;
  word_type* data_;
};

void swap(bitset& lhs, bitset& rhs) noexcept;
void swap(bitset::reference& lhs, bitset::reference rhs) noexcept;
void swap(bitset::iterator& lhs, bitset::iterator& rhs) noexcept;
void swap(bitset::view& lhs, bitset::view& rhs) noexcept;

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs);
bitset operator|(const bitset::const_view& lhs, const bitset::const_view& rhs);
bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs);
bitset operator~(const bitset::const_view& bs);
bitset operator<<(const bitset::const_view& bs, std::size_t count);
bitset operator>>(const bitset::const_view& bs, std::size_t count);

bool operator==(const bitset::const_view& left, const bitset::const_view& right);
bool operator!=(const bitset::const_view& left, const bitset::const_view& right);
std::string to_string(const bitset::const_view& bs);
std::string to_string(const bitset& bs);
std::ostream& operator<<(std::ostream& out, const bitset::const_view& bs);
