#ifndef ALICEO2_FOCAL_DECODER_H
#define ALICEO2_FOCAL_DECODER_H

#include <span>
#include <vector>

#include "DataFormatsFOCAL/DataStructs.h"
#include "DataFormatsFOCAL/Readout.h"

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
  int   id      = -1;
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
    /// Currently, this function expects the buffer to be no more
    /// than ONE event/trigger with ONE sample.
    /// On return, event data is available through getGBTLinks().
    void decodeBuffer(std::span<const char> buffer);

    /// \brief Get the decoded data for the last decoded event and sample
    /// \return Reference to std::array of GBTLinks with event data
    const std::vector<readout::GBTLink>& getGBTLinks() const;
  
  private:
    void processLine(const GBTLine& line);

    std::vector<readout::GBTLink> mGBTLinks     = {};
    std::vector<GBTLinkContext>   mLinkContexts = {};
};

} // namespace o2::focal

#endif 
