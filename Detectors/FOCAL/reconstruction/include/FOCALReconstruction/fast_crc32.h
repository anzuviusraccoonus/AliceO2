#ifndef ALICEO2_FOCAL_FASTCRC32_H
#define ALICEO2_FOCAL_FASTCRC32_H

#include <array>
#include <cstdint>

namespace o2::focal::fast_crc32
{

constexpr uint32_t polynomial = 0x04C11DB7; // see H2GCROC documentation

constexpr std::array<uint32_t, 256> make_table0() {
  std::array<uint32_t, 256> table{};

  for (size_t i = 0; i < 256; ++i) {
    uint32_t crc = i << 24;

    for (int bit = 0; bit < 8; ++bit) {
      if (crc & 0x80000000) {
        crc = (crc << 1) ^ polynomial;
      } else {
        crc <<= 1;
      }
    }

    table[i] = crc;
  }

  return table;
}

constexpr std::array<uint32_t, 256> make_next_table(const std::array<uint32_t, 256>& previous, const std::array<uint32_t, 256>& table0) {
  std::array<uint32_t, 256> table{};

  for (size_t i = 0; i < 256; ++i) {
    const uint32_t crc = previous[i];
    table[i] = (crc << 8) ^ table0[crc >> 24];
  }

  return table;
}

inline constexpr auto table0 = make_table0();
inline constexpr auto table1 = make_next_table(table0, table0);
inline constexpr auto table2 = make_next_table(table1, table0);
inline constexpr auto table3 = make_next_table(table2, table0);

inline uint32_t process_word(uint32_t crc, uint32_t word) noexcept {
  const uint32_t b0 = (word >> 24) & 0xFF;
  const uint32_t b1 = (word >> 16) & 0xFF;
  const uint32_t b2 = (word >> 8)  & 0xFF;
  const uint32_t b3 =  word        & 0xFF;

  return table3[ (crc >> 24)         ^ b0] ^
         table2[((crc >> 16) & 0xFF) ^ b1] ^
         table1[((crc >> 8)  & 0xFF) ^ b2] ^
         table0[ (crc        & 0xFF) ^ b3];
}

inline uint32_t calculate(const uint32_t* words) noexcept {
  uint32_t crc = 0;
  for (size_t i = 0; i < 39; ++i) {
    crc = process_word(crc, words[i]);
  }

  return crc;
}

} // namespace o2::focal::fast_crc32

#endif
