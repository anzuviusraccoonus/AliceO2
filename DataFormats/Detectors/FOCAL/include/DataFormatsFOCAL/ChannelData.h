#ifndef ALICEO2_FOCAL_CHANNELDATA_H
#define ALICEO2_FOCAL_CHANNELDATA_H


#include <cstdint>

#include "DataFormatsFOCAL/DataStructs.h"

namespace o2::focal 
{

enum class ChannelDataType {
  Undefined,             
  Characterization, // Case 4
  ADC,              // Case 1 & 2
  TOT               // Case 3
};

class ChannelData {
  public:
    ChannelData(DAQDataWord word, H2GCROCMode mode);

    DAQDataWord raw() const;
    H2GCROCMode mode() const;
    ChannelDataType type() const;

    bool tc() const;
    bool tp() const;
    
    bool hasADC() const;
    bool hasTOT() const;
    bool hasTOA() const;
    bool hasPrevADC() const;

    uint16_t adc() const;
    uint16_t prevAdc() const;
    uint16_t tot() const;
    uint16_t toa() const;

  private:
    DAQDataWord mWord;
    H2GCROCMode mMode;
};

} // namespace o2::focal

#endif
