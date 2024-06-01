#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace bitset_common {
using word_type = uint64_t;

template <typename T>
concept NonConst = !std::is_const_v<T>;

static constexpr word_type ALL_BITS = -1;
static constexpr word_type ZERO = 0;
static constexpr word_type ONE = 1;
static constexpr std::size_t WORD_BITS = std::numeric_limits<word_type>::digits;
} // namespace bitset_common
