#ifndef ALICEO2_FOCAL_EVENTDATA_H
#define ALICEO2_FOCAL_EVENTDATA_H

#include <vector>
#include <Rtypes.h>
#include "DataFormatsFOCAL/Readout.h"

namespace o2::focal
{

/// \class SampleData
/// \brief One sample (complete readout) of an event and it's associated link data
class SampleData {
  public:
    SampleData() = default;
    SampleData(int numLinks);

    std::vector<readout::GBTLink> gbtLinks;

  ClassDefNV(SampleData, 1);
};

/// \class EventData
/// \brief Simple class representing an event, a number of samples, and associated link data
class EventData {
  public:
    EventData() = default;
    EventData(int numLinks);

    /// \brief Adds a new sample with mNumGBTLinks number of links to hold data
    void addSample();

    /// \brief Get read-out data for specified sample index
    /// \param sampleIndex Index of sample to get data from (default: 0)
    /// \return Reference to vector of GBTLinks for specified sample
    /// Use this function to access parsed event data from different samples.
    /// In case the H2GCROC are ran in normal mode, there is only one sample
    /// recorded; then one can simply use ::getData() instead.
    std::vector<readout::GBTLink>& getData(size_t sampleIndex = 0);
    const std::vector<readout::GBTLink>& getData(size_t sampleIndex = 0) const;

    /// \brief Get the number of samples available in this event
    /// Will return the size of mSamples; the number of samples set up in this event.
    /// It does not guarantee that (all of) those samples are filled or complete.
    size_t getNumberOfSamples() const;

  private:
    int mNumGBTLinks = 0;
    std::vector<SampleData> mSamples;

  ClassDefNV(EventData, 1);
};

} // namespace o2::focal

#endif
