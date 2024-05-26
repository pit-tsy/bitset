#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

using word_type = uint64_t;

static constexpr word_type ALL_BITS = -1;
static constexpr word_type ZERO = 0;
static constexpr std::size_t WORD_BITS = std::numeric_limits<word_type>::digits;
