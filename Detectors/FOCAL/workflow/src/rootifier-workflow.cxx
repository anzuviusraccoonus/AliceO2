#include "Framework/ConfigParamSpec.h"
#include "FOCALWorkflow/RootifierSpec.h"

#include "Framework/runDataProcessing.h"

using namespace o2::focal;
using namespace o2::framework;

WorkflowSpec defineDataProcessing(ConfigContext const& cfg) {
  WorkflowSpec specs;
  specs.emplace_back(o2::focal::getRootifierSpec());

  return specs;
}
