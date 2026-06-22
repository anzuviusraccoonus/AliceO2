#ifndef ALICEO2_FOCAL_HCalROC_H
#define ALICEO2_FOCAL_HCalROC_H

#include <array>
#include <exception>
#include <string>
#include <vector>

#include <gsl/span>

#include "Rtypes.h"

#include "FOCALReconstruction/HCalROCDataLink.h"
#include "FOCALReconstruction/HCalDataWord.h"

namespace o2::focal 
{

class HCalROC {
  public:
    HCalROC() = default;
    ~HCalROC() = default;

    void fillHeaderData(HCalDataWord first, HCalDataWord second);
    void fillCommonModeData(HCalDataWord first, HCalDataWord second);
    void fillCalibrationData(HCalDataWord first, HCalDataWord second);
    void fillCRCData(HCalDataWord first, HCalDataWord second);
    void fillChannelData(HCalDataWord first, HCalDataWord second, int index);

    HCalROCDataLink getChipHalf(int index);
    void reset();

  private:
    std::array<HCalROCDataLink, 2> mHalves;
};

} // namespace o2::focal

#endif
