#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "Layout.h"

std::string str(std::vector<uint8_t> a) {
  std::ostringstream ss;
  bool first = true;
  for (auto i : a) {
    if (first) {
      ss << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(i);
      first = false;
    } else {
      ss << ' ' << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(i);
    }
  }
  return ss.str();
}

std::vector<uint8_t> extract_bits(std::vector<uint8_t> src, Layout layout) {
  size_t len = layout.byte_pos_end_ - layout.byte_pos_begin_ + 1;
  uint8_t r_mask = 0xff >> (7 - layout.bit_pos_begin_);

  if (len > sizeof(uint64_t) + 1) {
    throw std::out_of_range("out of range");
  }
  std::vector<uint8_t> dst(sizeof(uint64_t) + 1);
  std::cout << "dst.size()=" << dst.size() << std::endl;

  std::copy(src.begin() + layout.byte_pos_begin_,
            src.begin() + layout.byte_pos_end_ + 1,
            dst.begin() + dst.size() - len);
  dst[dst.size() - len] &= r_mask;
  if (layout.bit_pos_end_ > 0) {
    for (int i = 0; i <= len; i++) {
      uint8_t n = dst[sizeof(uint64_t) - i - 1] << (8 - layout.bit_pos_end_);
      dst[sizeof(uint64_t) - i] >>= layout.bit_pos_end_;
      dst[sizeof(uint64_t) - i] |= n;
    }
  }
  dst.erase(dst.begin());
  return dst;
}

bool Layout::check() const {
  if (bit_pos_begin_ >= 8 || bit_pos_end_ >= 8) {
    return false;
  }

  size_t begin = static_cast<size_t>(byte_pos_begin_) * 8 + 7 - bit_pos_begin_;
  size_t end = static_cast<size_t>(byte_pos_end_) * 8 + 7 - bit_pos_end_;

  return (end >= begin) && ((end - begin) <= sizeof(uint64_t) * 8);
}
size_t Layout::length() const {
  if (!check()) {
    return 0;
  }

  size_t begin =
      static_cast<size_t>(byte_pos_begin_) * 8 + (7 - bit_pos_begin_);
  size_t end = static_cast<size_t>(byte_pos_end_) * 8 + (7 - bit_pos_end_);

  return (begin <= end) ? (end - begin + 1) : 0;
}
