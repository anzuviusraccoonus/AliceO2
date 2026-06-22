#ifndef ALICEO2_FOCAL_HCalDataWord_H
#define ALICEO2_FOCAL_HCalDataWord_H

#include <cstdint>

namespace o2::focal {

  struct HCalDataWord {
    union {

      // DAQ header word
      struct {
        uint32_t tr : 4;  // trailer pattern (0101)
        uint32_t hm : 3;  // hamming decoding error bits
        uint32_t ob : 3;  // orbit counter
        uint32_t ec : 6;  // event counter
        uint32_t bx : 12; // bunch crossing counter
        uint32_t hd : 4;  // header pattern (1111)
      };

      // Channel data word
      struct {
        uint32_t toa : 10; // time of arrival
        uint32_t tot : 10; // time over threshold
        uint32_t adc : 10; // adc count
        uint32_t tp  : 1;  // "tot in progress" flag
        uint32_t tc  : 1;  // "tot complete" flag
      };

      uint32_t data;
    };

    constexpr operator uint32_t() const noexcept { return data; }

  };

  struct HCalDAQHeader : public HCalDataWord {
    HCalDAQHeader() {
      data = 0;
    }
    
    HCalDAQHeader(unsigned int w) {
      data = w;
    }
  };

  struct HCalChannel : public HCalDataWord {
    HCalChannel() {
      data = 0;
    }
    
    HCalChannel(unsigned int w) {
      data = w;
    }

  };

  // Struct representing a single line of payload: eight 32-bit words
  struct HCalGBTLine {
    HCalDataWord words[8]; 

    uint32_t hdr()      const { return  words[0]        & 0xFF ; }
    uint32_t link_id()  const { return (words[0] >> 8)  & 0xFF ; }
    uint32_t bx_cntr()  const { return (words[0] >> 16) & 0xFFF; }
    uint32_t ob_cntr()  const { return  words[1]               ; }
  };

} // namespace o2::focal

#endif 
