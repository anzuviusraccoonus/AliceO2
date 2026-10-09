#include <vector>

#include "DetectorsRaw/HBFUtilsInitializer.h"
#include "FOCALWorkflow/RawDecoderSpec.h"
#include "Framework/ConfigParamSpec.h"
#include "Framework/Variant.h"

using namespace o2::framework;
using namespace o2::focal;

void customize(std::vector<ConfigParamSpec>& workflowOptions) {
  std::vector<ConfigParamSpec> options{
    {"debug", VariantType::Bool, false, {"Enable debugging mode"}}
  };

  o2::raw::HBFUtilsInitializer::addConfigOption(options);
  workflowOptions.insert(workflowOptions.end(), options.begin(), options.end());
}

#include "Framework/runDataProcessing.h"

WorkflowSpec defineDataProcessing(ConfigContext const& cfg) {
  bool debug = cfg.options().get<bool>("debug");

  WorkflowSpec specs;
  specs.emplace_back(o2::focal::getRawDecoderSpec(debug));

  return specs;
}
