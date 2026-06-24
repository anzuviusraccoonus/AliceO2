#ifndef ALICEO2_FOCAL_HCALMAPPER_H
#define ALICEO2_FOCAL_HCALMAPPER_H

#include <string>
#include <fstream>

#include "DataFormatsFOCAL/Constants.h"

namespace o2::focal
{

  class HCalMapper {
    public:
      HCalMapper();
      ~HCalMapper();

      void init();
      std::pair<int, int> getRowCol(int gbtLink, int roc, int half, int chn);

		private:
      std::string mMapFilePath;
			std::pair<int, int> mMapping[constants::HCAL_NUM_GBT_LINKS]
																	[constants::HCAL_NUM_ROCS_PER_LINK]
																	[2]
																	[constants::HCAL_NUM_CHANNELS_PER_ROC_HALF];
		};


} // namespace o2::focal

#endif
