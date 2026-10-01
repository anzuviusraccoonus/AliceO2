#ifndef ALICEO2_FOCAL_DATASTRUCTS_H
#define ALICEO2_FOCAL_DATASTRUCTS_H

#include <array>
#include <cstdint>

namespace o2::focal 
{

  /// \struct Word
  /// \brief Base struct representing one 32-bit H2GCROC DAQ word
  struct Word {
    Word() = default;
    explicit constexpr Word(uint32_t w) : data{w} {}

    uint32_t data = 0;

    constexpr operator uint32_t() const noexcept { return data; }
  };

  /// \struct DAQHeaderWord
  /// \brief Helper class for DAQ header words
  struct DAQHeaderWord : public Word {
    DAQHeaderWord() = default;
    explicit constexpr DAQHeaderWord(uint32_t w) : Word{w} {}

    uint8_t   header()        const { return (data >> 28) & 0xF;    }
    uint16_t  bx_cntr()       const { return (data >> 16) & 0xFFF;  }
    uint8_t   ev_cntr()       const { return (data >> 10) & 0x3F;   }
    uint8_t   ob_cntr()       const { return (data >> 7 ) & 0x7;    }
    uint8_t   hamming_bits()  const { return (data >> 4 ) & 0x7;    }
    uint8_t   trailer()       const { return  data        & 0xF;    }
  };

  /// \struct DAQDataWord
  /// \brief Helper class for generic DAQ words
  /// The various getter functions assume that the contents 
  /// of the data word corresponds to "case 4" i.e.:
  /// 
  /// field: Tc  Tp  ADC  TOT  TOA
  ///  bits:  1   1   10   10   10
  ///
  /// which is the case if the H2GCROCs are run in 
  /// characterization mode. If not, it depends on the
  /// value of Tc:
  ///
  /// Case Tc == 0:
  /// 1 | Tp | ADC-1 | ADC | TOA
  ///
  /// Case Tc == 1:
  /// 0 | Tp | ADC-1 | TOT | TOA
  ///
  /// where ADC-1 is the ADC from the previous bunch crossing.
  /// See H2GCROC datasheet for more information.
  struct DAQDataWord : public Word {
    DAQDataWord() = default;
    explicit constexpr DAQDataWord(uint32_t w) : Word{w} {}

    bool      tc()  const { return (data >> 31) & 0x1;    }
    bool      tp()  const { return (data >> 30) & 0x1;    }
    uint16_t  adc() const { return (data >> 20) & 0x3FF;  }
    uint16_t  tot() const { return (data >> 10) & 0x3FF;  }
    uint16_t  toa() const { return  data        & 0x3FF;  }
  };

  /// \struct GBTLine
  /// \brief One packet, or "line", of 8 x 32-bit payload data
  struct GBTLine {
    std::array<uint32_t, 8> words;

    uint8_t   header()  const { return  words[0]        & 0xFF;   }
    uint8_t   link_id() const { return (words[0] >> 8 ) & 0xFF;   }
    uint16_t  bx_cntr() const { return (words[0] >> 16) & 0xFFF;  }
    uint32_t  ob_cntr() const { return  words[1];                 }
  };

} // namespace o2::focal

#endif
