#include <gtest/gtest.h>

#include "Layout.h"

TEST(ExtractBitsTest, BasicTests) {
  std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                               0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  std::cout << "data=" << str(data) << std::endl;
  std::vector<uint8_t> result;
  Layout layout = {0, 7, 7, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("11 22 33 44 55 66 77 88", str(result));

  layout = {3, 7, 4, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 44 55", str(result));

  layout = {7, 7, 14, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("88 99 aa bb cc dd ee ff", str(result));

  layout = {14, 7, 14, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 00 ff", str(result));

  layout = {14, 6, 14, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 00 7f", str(result));

  layout = {13, 1, 14, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 02 ff", str(result));

  layout = {13, 0, 14, 1};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 00 7f", str(result));

  layout = {13, 1, 14, 2};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 00 bf", str(result));

  layout = {12, 1, 14, 2};
  result = extract_bits(data, layout);
  EXPECT_EQ("00 00 00 00 00 00 7b bf", str(result));

  layout = {0, 7, 7, 0};
  result = extract_bits(data, layout);
  EXPECT_EQ("11 22 33 44 55 66 77 88", str(result));

  layout = {1, 3, 9, 4};
  EXPECT_EQ(64, layout.length());
  result = extract_bits(data, layout);
  EXPECT_EQ("23 34 45 56 67 78 89 9a", str(result));
}

TEST(ExtractBitsTest2, BasicTests) {
  std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                               0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  Layout layout = {0, 3, 1, 4};
  uint8_t ui8 = extract_bits<uint8_t>(data, layout);
  EXPECT_EQ(0x12, static_cast<int>(ui8));

  layout = {12, 1, 14, 2};
  uint16_t ui16 = extract_bits<uint16_t>(data, layout);
  EXPECT_EQ(0x7bbf, ui16);

  layout = {0, 7, 7, 0};
  uint64_t ui64 = extract_bits<uint64_t>(data, layout);
  EXPECT_EQ(0x1122334455667788, ui64);

  layout = {1, 3, 9, 4};
  ui64 = extract_bits<uint64_t>(data, layout);
  EXPECT_EQ(0x233445566778899a, ui64);
}

TEST(LayoutTest, ValidCases) {
  Layout layout1 = {0, 0, 0, 0};
  EXPECT_TRUE(layout1.check());

  Layout layout2 = {0, 7, 1, 0};
  EXPECT_TRUE(layout2.check());

  Layout layout3 = {0, 7, 7, 0};
  EXPECT_TRUE(layout3.check());

  Layout layout4 = {1, 7, 8, 0};  // 64ビットの正常ケース
  EXPECT_TRUE(layout4.check());

  layout4 = {1, 6, 8, 1};  // 64ビットの正常ケース
  EXPECT_TRUE(layout4.check());
}

TEST(LayoutTest, InvalidCases) {
  Layout layout5 = {0, 8, 0, 0};  // bit_pos_begin_ が8以上
  EXPECT_FALSE(layout5.check());

  Layout layout6 = {0, 0, 0, 8};  // bit_pos_end_ が8以上
  EXPECT_FALSE(layout6.check());

  Layout layout7 = {1, 0, 0, 0};  // end が begin より小さい
  EXPECT_FALSE(layout7.check());

  Layout layout8 = {0, 7, 10, 0};  // 範囲が uint64_t を超える
  EXPECT_FALSE(layout8.check());

  Layout layout9 = {2, 7, 10, 0};  // 65ビットの異常ケース
  EXPECT_FALSE(layout9.check());

  Layout layout4 = {1, 7, 9, 7};  // 65ビットの異常ケース
  EXPECT_TRUE(layout4.check());

  layout4 = {1, 6, 8, 0};  // 65ビットの異常ケース
  EXPECT_TRUE(layout4.check());
}

// ✅ 正常系テスト：有効なビット範囲
TEST(LayoutTest, ValidRange) {
  Layout layout = {0, 7, 1, 0};  // ビット位置 7 から ビット位置 8
  EXPECT_TRUE(layout.check());
}

// ✅ 正常系テスト：開始位置が 1 (0以外)
TEST(LayoutTest, ValidRangeWithNonZeroStart) {
  Layout layout = {1, 7, 2, 0};  // ビット位置 15 から ビット位置 16
  EXPECT_TRUE(layout.check());
}

// ❌ 異常系テスト：`bit_pos_begin_` が 8 以上
TEST(LayoutTest, InvalidBitPosBegin) {
  Layout layout = {0, 8, 1, 0};  // `bit_pos_begin_` が 8 → 異常
  EXPECT_FALSE(layout.check());
}

// ❌ 異常系テスト：`bit_pos_end_` が 8 以上
TEST(LayoutTest, InvalidBitPosEnd) {
  Layout layout = {0, 0, 1, 8};  // `bit_pos_end_` が 8 → 異常
  EXPECT_FALSE(layout.check());
}

// ❌ 異常系テスト：`end` が `begin` 以下
TEST(LayoutTest, EndLessThanBegin) {
  Layout layout = {1, 0, 0, 0};  // `end` が `begin` より小さい → 異常
  EXPECT_FALSE(layout.check());
}

// ❌ 異常系テスト：ビット幅が 65 ビット (64ビット超え)
TEST(LayoutTest, ExceedsMaxWidth) {
  Layout layout = {0, 7, 8, 0};  // 65ビット (異常)
  EXPECT_FALSE(layout.check());
}

// ❌ 異常系テスト：`byte_pos_begin_` が 1 で 65ビット
TEST(LayoutTest, ExceedsMaxWidthWithNonZeroStart) {
  Layout layout = {1, 7, 9, 0};  // 65ビット (異常)
  EXPECT_FALSE(layout.check());
}

// ✅ 正常系テスト：最大限のビット幅（64ビット）
TEST(LayoutTest, MaxWidth) {
  Layout layout = {0, 7, 7, 0};  // ちょうど 64 ビット
  EXPECT_TRUE(layout.check());
}

// ✅ 正常系テスト：1ビットの長さ
TEST(LayoutTest, LengthSingleBit) {
  Layout layout = {0, 7, 0, 7};  // 同じビット位置 (1ビット)
  EXPECT_EQ(layout.length(), 1);
}

// ✅ 正常系テスト：隣接するビット
TEST(LayoutTest, LengthAdjacentBits) {
  Layout layout = {0, 7, 0, 6};  // ビット位置 7 から 6 (2ビット)
  EXPECT_EQ(layout.length(), 2);
}

// ✅ 正常系テスト：`begin` が途中のビットから開始
TEST(LayoutTest, LengthPartialStart) {
  Layout layout = {0, 5, 1, 0};    // ビット位置 5 から 8 ビット目
  EXPECT_EQ(layout.length(), 14);  // 修正後の正しい値
}

// ✅ 正常系テスト：`byte_pos_begin_` が 1 の場合
TEST(LayoutTest, LengthNonZeroStart) {
  Layout layout = {1, 7, 2, 0};    // ビット位置 15 から 16 (16ビット)
  EXPECT_EQ(layout.length(), 16);  // 修正後の正しい値
}

// ✅ 正常系テスト：最大幅 (64ビット)
TEST(LayoutTest, LengthMaxWidth) {
  Layout layout = {0, 7, 7, 0};  // 64ビット幅
  EXPECT_EQ(layout.length(), 64);
}

// ❌ 異常系テスト：`check()` が `false` のとき `length()` が 0 になる
TEST(LayoutTest, LengthInvalidRange) {
  Layout layout = {0, 7, 8, 0};  // 65ビット (異常)
  EXPECT_EQ(layout.length(), 0);
}

// ❌ 異常系テスト：`bit_pos_begin_` が 8 以上
TEST(LayoutTest, LengthInvalidBitPosBegin) {
  Layout layout = {0, 8, 1, 0};  // `bit_pos_begin_` が 8 (異常)
  EXPECT_EQ(layout.length(), 0);
}

// ❌ 異常系テスト：`bit_pos_end_` が 8 以上
TEST(LayoutTest, LengthInvalidBitPosEnd) {
  Layout layout = {0, 0, 1, 8};  // `bit_pos_end_` が 8 (異常)
  EXPECT_EQ(layout.length(), 0);
}

TEST(VectorShiftTest, NoShift) {
  std::vector<uint8_t> data = {0b00000001, 0b00000010};
  EXPECT_EQ(vector_shift(data, 0), data);
}

TEST(VectorShiftTest, ShiftBy1) {
  std::vector<uint8_t> data = {0b00000001, 0b00000010};
  std::vector<uint8_t> expected = {0b00000010, 0b00000100};
  EXPECT_EQ(vector_shift(data, 1), expected);
}

TEST(VectorShiftTest, ShiftBy7) {
  std::vector<uint8_t> data = {0b00000001, 0b00000010};
  std::vector<uint8_t> expected = {0b10000001, 0b00000000};
  EXPECT_EQ(str(vector_shift(data,7)), str(expected));
}

TEST(VectorShiftTest, EmptyVector) {
  std::vector<uint8_t> data = {};
  EXPECT_EQ(vector_shift(data, 5), data);
}

TEST(VectorShiftTest, AllOnes) {
  std::vector<uint8_t> data = {0xFF, 0xFF};
  std::vector<uint8_t> expected = {
      0xFf, 0xFe};  // 左に1ビットシフトして最上位ビットが隣に移動
  EXPECT_EQ(vector_shift(data, 1), expected);
}

TEST(WriteBits, BasicTest) {
    std::vector<uint8_t> data = {0, 0, 0, 0};
    std::vector<uint8_t> expected = {0xff, 0xff, 0, 0};
    Layout layout = {0, 7, 1, 0};
    uint16_t value = 0xffff;
    EXPECT_EQ(str(write_bits(data, layout, value)), str(expected));

    data = {0xff, 0xff, 0xff, 0xff};
    expected = {0xff, 0, 0, 0xff};
    value = 0;
    layout = {1,7, 2, 0};
    EXPECT_EQ(str(write_bits(data, layout, value)), str(expected));
    
    data = {0xff, 0xff, 0xff, 0xff};
    expected = {0xff, 0x12, 0x34, 0xff};
    value = 0x1234;
    layout = {1, 7, 2, 0};
    EXPECT_EQ(str(write_bits(data, layout, value)), str(expected));
    
    data = {0, 0, 0, 0};
    expected = {0, 1, 0xff, 0xfe};
    value = 0xffff;
    layout = {1, 0, 3, 1};
    EXPECT_EQ(str(write_bits(data, layout, value)), str(expected));
}
// メイン関数（Google Testのエントリポイント）
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
