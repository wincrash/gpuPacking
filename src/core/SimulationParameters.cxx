#include "SimulationParameters.h"
#include "core/Parameters.h"
#include <iostream>
void InitSimulationParameters(SimulationParameters &data) {
  auto result = Parameters::getInstance()->getResults();
  data.AREA=result["area"].as<double>();
  data.PRINT_TIMES_SKIP = result["timer"].as<int>();
  data.WRITE_RESULTS_SKIP = result["writer"].as<int>();
  data.CONTACT_SEARCH_SKIP = result["search"].as<int>();
  data.END = result["simulation-time"].as<double>();
  data.DELTA = result["mechanical-time-step"].as<double>();
  data.RO = result["materials-RO"].as<std::vector<double>>();

  data.E= result["materials-E"].as<std::vector<double>>();
  data.NIU = result["materials-NIU"].as<std::vector<double>>();
  for(int i=0;i<data.E.size();i++)
  {
    std::cout<<"Material "<<i<<" E = "<<data.E[i]<<" NIU = "<<data.NIU[i]<<"\n";
  }
  data.FRICTION = result["FRICTION"].as<std::vector<double>>();
  data.MAT_ID_1 = result["ID1"].as<std::vector<int>>();
  data.MAT_ID_2 = result["ID2"].as<std::vector<int>>();
  data.GRAVITATION = result["gravitation"].as<std::vector<double>>();
  data.NORMAL_DAMPING = result["normal-damping"].as<double>();
  data.TANGENTIAL_DAMPING = result["tangential-damping"].as<double>();
  data.ROLLING_FRICTION = result["rolling-friction"].as<double>();

  std::vector<double> bounds = result["bounds"].as<std::vector<double>>();
  data.BOUNDS_MIN = MAKE_REAL4(bounds[0], bounds[2], bounds[4], 0);
  data.BOUNDS_MAX = MAKE_REAL4(bounds[1], bounds[3], bounds[5], 0);

  data.WALLS_MIN = data.BOUNDS_MIN;
  data.WALLS_MAX = data.BOUNDS_MAX;
  data.WALL_SPEED = result["wall-speeds"].as<std::vector<double>>();
  data.speed_startup=result["speed-startup"].as<int>();
   
}
