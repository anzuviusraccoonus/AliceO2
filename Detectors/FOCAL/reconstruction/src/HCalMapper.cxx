#include <filesystem>
#include <TSystem.h>
#include <fairlogger/Logger.h>
#include "FOCALReconstruction/HCalMapper.h"

using namespace o2::focal;

HCalMapper::HCalMapper() {
  // Initialize the array to pairs of (-1,-1)
  // These can be used to test whether a channel was mapped or not
  std::memset(mMapping, -1, sizeof(mMapping));

  // Get the path to the mapping file
  // TODO: move this to a config?
  mMapFilePath = Form("%s/share/Detectors/FOC/files/mapping_hcal.data", 
                      gSystem->Getenv("O2_ROOT"));

  init();
}

HCalMapper::~HCalMapper() {
}

void HCalMapper::init() {
  if (not std::filesystem::exists(mMapFilePath)) {
    LOGF(error, "HCal channel mapping file %s does not exist or can't be opened", mMapFilePath);
    return;
  }

  std::ifstream infile(mMapFilePath);
  std::string line;
  int lineNumber = 0;
  int numMappedChannels = 0;
  while (std::getline(infile, line)) {
    ++lineNumber;
    if (line.empty() || line.starts_with("//")) {
      continue;
    }

    std::istringstream iss(line);
    int row, col, link, roc, half, chn;
    if ( !(iss >> row >> col >> link >> roc >> half >> chn) ) {
      LOGF(warn, "Malformed line in HCal channel mapping file (line %d)", lineNumber);
      continue;
    }

    mMapping[link][roc][half][chn].first  = row;
    mMapping[link][roc][half][chn].second = col;
    ++numMappedChannels;
  }

  LOGF(info, "HCalMapper finished loading mapping file; %d channels were mapped.", numMappedChannels);

}

std::pair<int, int> HCalMapper::getRowCol(int link, int roc, int half, int chn) {
  return mMapping[link][roc][half][chn];
}
