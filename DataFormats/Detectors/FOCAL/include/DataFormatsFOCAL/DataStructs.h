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
  /// \brief Helper class for generic DAQ data words
  struct DAQDataWord : public Word {
    DAQDataWord() = default;
    explicit constexpr DAQDataWord(uint32_t w) : Word{w} {}

    bool      bit31()  const { return (data >> 31) & 0x1;    }
    bool      bit30()  const { return (data >> 30) & 0x1;    }
    uint16_t  field1() const { return (data >> 20) & 0x3FF;  }
    uint16_t  field2() const { return (data >> 10) & 0x3FF;  }
    uint16_t  field3() const { return  data        & 0x3FF;  }
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

  /// \enum GBTLineType
  /// \brief Types of interpretations of packets
  /// "Idle" are those packets resulting from an IDLE command,
  /// which is also expected to be found at least 1x at end-of-frame.
  /// "DAQHeader" are the first packets of a DAQ frame.
  /// "Padding" are packets that are all zeroes and can be safely ignored.
  /// "Trigger" packets may be put in as the first packet of a new event,
  /// though they don't actually originate from a readout LpGBT link.
  enum class GBTLineType {
    Generic,
    Trigger,
    Idle,
    DAQHeader,
    Padding
  };

  /// \enum H2GCROCMode
  /// \brief Operational mode of the H2GCROC chip
  enum class H2GCROCMode {
    Normal,
    Characterization
  };

  /// \brief Determine the type of a packet or "line" of payload data
  /// \return GBTLineType with a non-Generic value if line matches criterion
  GBTLineType classify(const GBTLine& line);

} // namespace o2::focal

#endif
