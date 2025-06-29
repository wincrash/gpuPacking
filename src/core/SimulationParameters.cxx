#include "SimulationParameters.h"
#include "core/Parameters.h"
#include <iostream>
void InitSimulationParameters(SimulationParameters &data) {
  auto result = Parameters::getInstance()->getResults();
  data.PRINT_TIMES_SKIP = result["timer"].as<int>();
  data.WRITE_RESULTS_SKIP = result["writer"].as<int>();
  data.CONTACT_SEARCH_SKIP = result["search"].as<int>();
  data.END = result["simulation-time"].as<double>();
  data.DELTA = result["mechanical-time-step"].as<double>();

  std::vector<double> bounds = result["bounds"].as<std::vector<double>>();
  data.BOUNDS_MIN = MAKE_REAL4(bounds[0], bounds[2], bounds[4], 0);
  data.BOUNDS_MAX = MAKE_REAL4(bounds[1], bounds[3], bounds[5], 0);

  data.WALLS_MIN = data.BOUNDS_MIN;
  data.WALLS_MAX = data.BOUNDS_MAX;
   
}
