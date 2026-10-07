#ifndef ALICEO2_FOCAL_DECODER_H
#define ALICEO2_FOCAL_DECODER_H

#include <span>
#include <vector>

#include "DataFormatsFOCAL/DataStructs.h"
#include "DataFormatsFOCAL/Readout.h"
#include "DataFormatsFOCAL/EventData.h"

namespace o2::focal 
{

/// \struct GBTLinkContext
/// \brief State machine for decoder algorithm
struct GBTLinkContext {
  enum class State {
    WaitingForFrame,
    ReadingFrame,
    EndOfFrame,
    Finished,
    Errored,
  };

  State state   = State::WaitingForFrame;
  int   lines   = 0;
  int   samples = 0;
  int   id      = -1; // currently unusued
};

/// \class Decoder
/// \brief Decoder algorithm for H2GCROC payload data
class Decoder {
  public:
    Decoder(int numLinks);
    ~Decoder() = default;

    /// \brief Reset the decoder to initial state
    /// This will set all data on links to zero, as well
    /// as resetting the GBTLinkContext state matchines.
    void reset();

    /// \brief Decodes all data the current buffer
    /// Casts the buffer to a vector of GBTLines (32 byte packets)
    /// and iterates over them, reading the event data present.
    void decodeBuffer(std::span<const char> buffer);

    /// \brief Get the decoded data for the last decoded event
    const EventData& getEvent() const;

    /// \brief Get the decoded data for a specific event
    const EventData& getEvent(int index) const;

    /// \brief Get the decoded data for all events
    const std::vector<EventData>& getEvents() const;
  
  private:
    void processLine(const GBTLine& line);
    void prepareNewEvent();

    std::vector<GBTLinkContext> mLinkContexts = {};

    uint8_t mNumGBTLinks;
    EventData* mCurrentEvent;
    std::vector<EventData> mEvents;
};

} // namespace o2::focal

#endif 
