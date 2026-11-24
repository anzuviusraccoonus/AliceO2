#ifndef ALICEO2_FOCAL_ROOTRECORDS_H
#define ALICEO2_FOCAL_ROOTRECORDS_H

#include <cstdint>
#include <vector>

#include <Rtypes.h>

#include "DataFormatsFOCAL/EventData.h"

namespace o2::focal::records 
{

struct DataLinkRecord {
  uint8_t cruIndex;
  uint8_t sampleIndex;
  uint8_t gbtIndex;
  uint8_t readoutChipIndex;
  uint8_t dataLinkIndex;

  uint32_t header;
  uint32_t commonMode;
  uint32_t calibration;
  uint32_t crc;
  
  ClassDefNV(DataLinkRecord, 1);
};

struct ChannelRecord {
  uint8_t cruIndex;
  uint8_t sampleIndex;
  uint8_t gbtIndex;
  uint8_t readoutChipIndex;
  uint8_t dataLinkIndex;
  uint8_t channelIndex;

  bool bit31;
  bool bit30;
  uint16_t field1;
  uint16_t field2;
  uint16_t field3;
  
  ClassDefNV(ChannelRecord, 1);
};

struct Event {
  std::vector<DataLinkRecord> dataLinks;
  std::vector<ChannelRecord> channels;

  void clear() {
    dataLinks.clear();
    channels.clear();
  }

  ClassDefNV(Event, 1);
};

void flattenEvent(const CombinedEventData& input, records::Event& output);

} // namespace o2::focal::records

#endif
