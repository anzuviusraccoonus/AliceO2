#include "DataFormatsFOCAL/ROOTEventRecords.h"
#include "DataFormatsFOCAL/Readout.h"
#include "DataFormatsFOCAL/EventData.h"

//namespace
//{
//
//template<typename T>
//uint32_t recordIndex(const std::vector<T>& records) {
//  if (records.size() >= std::numeric_limits<uint32_t>::max()) {
//    throw std::overflow_error("Too many records in event");
//  }
//
//  return static_cast<uint32_t>(records.size());
//}
//
//} // namespace

using namespace o2::focal;
using namespace o2::focal::readout;
void records::flattenEvent(const CombinedEventData& input, records::Event& output) {
  output.clear();

  const std::vector<EventData>& cruData = input.getCRUData();
  for (size_t cru = 0; cru < cruData.size(); ++cru) {
    const EventData& data = cruData[cru];

    for (size_t sample = 0; sample < data.getNumberOfSamples(); ++sample) {
      const auto& sampleData = data.getData(sample);

      for (size_t gbt = 0; gbt < sampleData.size(); ++gbt) {
        const GBTLink& gbtLink = sampleData[gbt];

        for (size_t roc = 0; roc < 3; ++roc) {
          const ReadoutChip& readoutChip = gbtLink.getROC(roc);

          for (size_t link = 0; link < 2; ++link) {
            const DataLink& datalink = readoutChip.getDataLink(link);
            output.dataLinks.push_back({cru,
                                        sample,
                                        gbt,
                                        roc,
                                        link,
                                        datalink.getHeader(),
                                        datalink.getCommonMode(),
                                        datalink.getCalibration(),
                                        datalink.getCRC()
                                        });

            for (size_t chn = 0; chn < 36; ++chn) {
              const DAQDataWord& channel = datalink.getChannel(chn);
              output.channels.push_back({cru,
                                         sample,
                                         gbt,
                                         roc,
                                         link,
                                         chn,
                                         channel.bit31(),
                                         channel.bit30(),
                                         channel.field1(),
                                         channel.field2(),
                                         channel.field3()
                                         });
            }
          }
        }
      }
    }
  }
}
