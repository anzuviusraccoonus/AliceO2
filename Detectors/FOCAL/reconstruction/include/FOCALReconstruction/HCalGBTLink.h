#ifndef ALICEO2_FOCAL_HCalGBTLink_H
#define ALICEO2_FOCAL_HCalGBTLink_H

#include <array>
#include <exception>
#include <string>
#include <vector>

#include <gsl/span>

#include "Rtypes.h"

#include "DataFormatsFOCAL/Constants.h"
#include "FOCALReconstruction/HCalROC.h"

namespace o2::focal 
{

class HCalGBTLink {
  public:
    HCalGBTLink() = default;
    ~HCalGBTLink() = default;

    void reset();
    void fillData(HCalGBTLine line, int lineNumber);
    HCalROC getROC(int index);

  private:
    std::array<HCalROC, constants::HCAL_NUM_ROCS_PER_LINK> mROCs;
};

} // namespace o2::focal

#endif
