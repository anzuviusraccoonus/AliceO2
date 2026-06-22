#ifndef ALICEO2_FOCAL_HCalROCDataLink_H
#define ALICEO2_FOCAL_HCalROCDataLink_H

#include <array>
#include <exception>
#include <string>
#include <vector>

#include <gsl/span>

#include "Rtypes.h"

#include "DataFormatsFOCAL/Constants.h"
#include "FOCALReconstruction/HCalDataWord.h"

namespace o2::focal 
{

class HCalROCDataLink {
  public:
    HCalROCDataLink() = default;
    ~HCalROCDataLink() = default;

    void setHeader(unsigned int data);
    void setCommonMode(unsigned int data);
    void setCalibration(unsigned int data);
    void setChannel(unsigned int data, int index);
    void setCRC(unsigned int data);
    
    HCalDAQHeader getHeader();
    HCalChannel getCommonMode();
    HCalChannel getCalibration();
    HCalChannel getChannel(int index);
    unsigned int getCRC();

  private:
    HCalDAQHeader mHeader;
    HCalChannel mChannels[constants::HCAL_NUM_CHANNELS_PER_ROC_HALF];
    HCalChannel mCommonMode;
    HCalChannel mCalibration;
    unsigned int mCRC;
};

} // namespace o2::focal

#endif
