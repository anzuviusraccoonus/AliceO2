#ifndef ALICEO2_FOCAL_READOUT_H
#define ALICEO2_FOCAL_READOUT_H

#include <array>
#include <cstddef>
#include <span>

#include "DataFormatsFOCAL/DataStructs.h"

namespace o2::focal::readout
{
  
  /// \class DataLink
  /// \brief H2GCROC eLink abstraction layer
  class DataLink {
    public:
      static constexpr size_t NUM_WORDS = 40;
      static constexpr size_t NUM_CHANNELS = 36;

      void setHeader(uint32_t data);
      void setCommonMode(uint32_t data);
      void setCalibration(uint32_t data);
      void setCRC(uint32_t data);
      void setChannel(uint32_t data, size_t index);

      DAQHeaderWord getHeader() const;
      DAQDataWord getCommonMode() const;
      DAQDataWord getCalibration() const;
      DAQDataWord getCRC() const;
      DAQDataWord getChannel(size_t index) const;

    private:
      DAQHeaderWord mHeader;
      DAQDataWord mCommonMode;
      DAQDataWord mCalibration;
      DAQDataWord mCRC;
      std::array<DAQDataWord, NUM_CHANNELS> mChannels;
  };

  /// \class ReadoutChip
  /// \brief H2GCROC abstraction layer
  class ReadoutChip {
    public:
      static constexpr size_t NUM_DATALINKS = 2;

      /// \brief Reset this ReadoutChip and all it's DataLinks
      /// Re-initializes the instance, which will result in all data
      /// members of all DataLinks to be set to 0.
      void reset();

      /// \brief Fill data for the DAQ header word (word 0) for given DataLink index
      /// \param data The 32-bit word containing DAQ header data
      /// \param datalink Index of DataLink to fill data on
      void fillHeaderData(uint32_t data, size_t datalink);

      /// \brief Fill data for the common mode channel (word 1) for given DataLink index
      /// \param data The 32-bit word containing common mode data
      /// \param datalink Index of DataLink to fill data on
      void fillCommonModeData(uint32_t data, size_t datalink);

      /// \brief Fill data for the calibration channel (word 20) for given DataLink index
      /// \param data The 32-bit word containing calibration data
      /// \param datalink Index of DataLink to fill data on
      void fillCalibrationData(uint32_t data, size_t datalink);

      /// \brief Fill data for the CRC checksum channel (word 39) for given DataLink index
      /// \param data The 32-bit word containing the CRC checksum
      /// \param datalink Index of DataLink to fill data on
      void fillCRCData(uint32_t data, size_t datalink);

      /// \brief Fill data for a given readout channel index for given DataLink index
      /// \param data The 32-bit word containing the channel data
      /// \param datalink Index of DataLink to fill data on
      /// \param index Index of channel to fill data to
      void fillChannelData(uint32_t data, size_t datalink, size_t index);
      
      /// \brief Get a view of all data links belonging to this ReadoutChip
      /// \return std::span of DataLinks
      std::span<DataLink> getDataLinks();

    private:
      std::array<DataLink, NUM_DATALINKS> mDataLinks;
  };

  /// \class GBTLink
  /// \brief LpGBT link abstraction layer
  class GBTLink {
    public:
      static constexpr size_t NUM_READOUTCHIPS = 3;

      /// \brief Reset this LpGBT link and all it's ReadoutChips
      /// Re-initializes the instance, which will result in all data
      /// members of all DataLinks to be set to 0.
      void reset();

      /// \brief Fill event data for one LpGBT link packet
      /// \param line A GBTLine (8 x 32 bit words) of payload data
      /// \param lineNumber Which line/packet number this is
      /// Takes in one "line" of payload data; 8 words of 32 bit width,
      /// corresponding to one package of data from a given LpGBT link,
      /// and stores the data in the relevant event data buffers.
      void fillData(const GBTLine& line, size_t lineNumber);

      /// \brief Get a view of all data links belonging to this LpGBT link
      /// \return std::view of DataLinks
      auto getDataLinks();

    private:
      std::array<ReadoutChip, NUM_READOUTCHIPS> mReadoutChips;
  };

} // namespace o2::focal::readout

#endif
