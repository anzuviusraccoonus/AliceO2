#ifndef ALICEO2_FOCAL_EVENTDATA_H
#define ALICEO2_FOCAL_EVENTDATA_H

#include <vector>

#include "DataFormatsFOCAL/Readout.h"

namespace o2::focal
{

/// \struct SampleData
/// \brief One sample (complete readout) of an event and it's associated link data
struct SampleData {
  SampleData(int numLinks);

  std::vector<readout::GBTLink> gbtLinks;
};

/// \class EventData
/// \brief Simple class representing an event, a number of samples, and associated link data
class EventData {
  public:
    EventData();
    EventData(int numLinks);

    /// \brief Adds a new sample with mNumGBTLinks number of links to hold data
    void addSample();

    std::vector<SampleData> samples;

  private:
    int mNumGBTLinks;
};

} // namespace o2::focal

#endif
