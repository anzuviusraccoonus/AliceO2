#ifndef ALICEO2_FOCAL_MAPPER_H
#define ALICEO2_FOCAL_MAPPER_H

#include <array>
#include <cstdint>
#include <string>

#include "DataFormatsFOCAL/Readout.h"

namespace o2::focal
{

class Mapper {
  public:
    Mapper(const std::string mapping);
    ~Mapper();

    void init(const std::string mapping);

    int8_t getRow(int gbtlink, int roc, int datalink, int channel) const;
    int8_t getCol(int gbtlink, int roc, int datalink, int channel) const;

    std::pair<int8_t, int8_t> getRowCol(int gbtlink, int roc, int datalink, int channel) const;

  private:

    // maybe represent a channel address with one number instead of indexing every connection?
    using ChannelCoordinates  = std::pair<int8_t, int8_t>;
    using DataLinkChannels    = std::array<ChannelCoordinates,  readout::DataLink::NUM_CHANNELS>;
    using ROCDataLinks        = std::array<DataLinkChannels,    readout::ReadoutChip::NUM_DATALINKS>;
    using GBTLinkROCs         = std::array<ROCDataLinks,        readout::GBTLink::NUM_READOUTCHIPS>;
    using DetectorLinks       = std::array<GBTLinkROCs,         24>; // TODO: 24 links per CRU card, define this somewhere?

    static DetectorLinks makeDefaultMapping() {
      DetectorLinks mapping{};
      for (auto& gbt : mapping) {
        for (auto& roc : gbt) {
          for (auto& lnk : roc) {
            lnk.fill(ChannelCoordinates{-1, -1});
          }
        }
      }

      return mapping;
    }

    DetectorLinks mMapping = makeDefaultMapping(); // will be a four-dimensional array of 2-tuples
};

} // namespace o2::focal

#endif
