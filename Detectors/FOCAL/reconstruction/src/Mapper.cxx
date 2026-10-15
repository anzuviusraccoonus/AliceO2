#include <filesystem>
#include <istream>

#include "Framework/Logger.h"

#include "FOCALReconstruction/Mapper.h"

using namespace o2::focal;

Mapper::Mapper(const std::string mapping) {
  init(mapping);
}

Mapper::~Mapper() {
}

void Mapper::init(const std::string mapping) {
  if (not std::filesystem::exists(mapping)) {
    LOGF(error, "File '%s' does not exist or can't be opened", mapping);
    return;
  }
  
  std::ifstream infile(mapping);
  std::string line;
  int lineNumber = 0;
  int numMappedChannels = 0;
  int numBadLines = 0;
  while (std::getline(infile, line)) {
    ++lineNumber;
    if (line.empty() || line.starts_with("//")) {
      continue;
    }

    std::istringstream iss(line);
    int row, col, gbt, roc, lnk, chn;
    if (!(iss >> row >> col >> gbt >> roc >> lnk >> chn)) {
      ++numBadLines;
      LOGF(warn, "Malformed line in mapping file (line: %d)", lineNumber);
      continue;
    }

    mMapping[gbt][roc][lnk][chn].first  = row;
    mMapping[gbt][roc][lnk][chn].second = col;
    ++numMappedChannels;
  }

  constexpr size_t numChannels = 24 * readout::GBTLink::NUM_READOUTCHIPS *
                                      readout::ReadoutChip::NUM_DATALINKS *
                                      readout::DataLink::NUM_CHANNELS;

  LOGF(info, "Finished loading mapping file. %d out of %d channels were mapped (%d malformed entries skipped)", numMappedChannels, numChannels, numBadLines);
}

int8_t Mapper::getRow(int gbtlink, int roc, int datalink, int channel) const {
  return mMapping[gbtlink][roc][datalink][channel].first;
}

int8_t Mapper::getCol(int gbtlink, int roc, int datalink, int channel) const {
  return mMapping[gbtlink][roc][datalink][channel].second;
}

std::pair<int8_t, int8_t> Mapper::getRowCol(int gbtlink, int roc, int datalink, int channel) const {
  return mMapping[gbtlink][roc][datalink][channel];
}
