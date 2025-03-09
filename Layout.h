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

// C++20以前の環境用のカスタム実装
template <typename T>
constexpr T byteswap(T value) {
  static_assert(std::is_integral<T>::value,
                "The return type must be of integer type.");

  if constexpr (sizeof(T) == 1) {
    return value;  // 8bitはそのまま返す
  } else if constexpr (sizeof(T) == 2) {
    return (value >> 8) | (value << 8);
  } else if constexpr (sizeof(T) == 4) {
    return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24);
  } else if constexpr (sizeof(T) == 8) {
    return ((value & 0x00000000000000FF) << 56) |
           ((value & 0x000000000000FF00) << 40) |
           ((value & 0x0000000000FF0000) << 24) |
           ((value & 0x00000000FF000000) << 8) |
           ((value & 0x000000FF00000000) >> 8) |
           ((value & 0x0000FF0000000000) >> 24) |
           ((value & 0x00FF000000000000) >> 40) |
           ((value & 0xFF00000000000000) >> 56);
  } else {
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "Unsupported size");
    return value;  // フォールバック (通常は到達しない)
  }
}
inline bool isLittleEndian() {
  int i = 1;
  uint8_t *c = reinterpret_cast<uint8_t *>(&i);
  return (*c == 1);
}

template <typename T>
T extract_bits(std::vector<uint8_t> src, Layout layout) {
  static_assert(std::is_integral<T>::value,
                "The return type must be of integer type.");
  std::vector<uint8_t> data = extract_bits(src, layout);
  uint64_t *x = reinterpret_cast<uint64_t *>(data.data()), result;
  if (isLittleEndian()) {
    result = byteswap(*x);
  }
  else {
    result = *x;
  }
  std::cout << "isLittleEndian()=" << isLittleEndian() << " data=" << str(data) << " result=" << std::hex << result << std::endl;
  return static_cast<T>(result);
}
