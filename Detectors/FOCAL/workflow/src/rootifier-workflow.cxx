#include "DataFormatsFOCAL/EventData.h"
#include "Framework/ConfigParamSpec.h"
#include "FOCALWorkflow/RootifierSpec.h"

using namespace o2::focal;

#include "Framework/runDataProcessing.h"

using namespace o2::framework;

#include <TStreamerInfo.h>

WorkflowSpec defineDataProcessing(ConfigContext const& cfg) {
  //TStreamerInfo::SetStreamMemberWise(false);
  WorkflowSpec specs;
  specs.emplace_back(o2::focal::getRootifierSpec());

  return specs;
}
