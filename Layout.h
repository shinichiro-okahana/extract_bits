#pragma once

struct Layout {
  uint8_t byte_pos_begin_;
  uint8_t bit_pos_begin_;
  uint8_t byte_pos_end_;
  uint8_t bit_pos_end_;

  bool check() const;
  size_t length() const;
};

std::string str(std::vector<uint8_t> a);
std::vector<uint8_t> extract_bits(std::vector<uint8_t> src, Layout layout);
