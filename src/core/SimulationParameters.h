#pragma once
#include "core/DataTypes.h"

class SimulationParameters {
public:
  REAL ctime = 0;
  ULONG cstep = 0;
  REAL END = 1.0;
  REAL DELTA = 1.E-5;
  REAL MIN_RADIUS = 0;
  REAL MAX_RADIUS = 0;
  REAL SKIN = 1.01;
  REAL4 BOUNDS_MIN = MAKE_REAL4(100000, 100000, 100000, 0);
  REAL4 BOUNDS_MAX = MAKE_REAL4(-100000, -100000, -100000, 0);
  REAL4 WALLS_MIN;
  REAL4 WALLS_MAX;


  bool COMPUTE = true;
  bool CONTACT_SEARCH = true;
  bool WRITE_RESULTS = true;
  bool PRINT_TIMES = true;

  int CONTACT_SEARCH_SKIP = 10;
  int WRITE_RESULTS_SKIP = 1000;
  int PRINT_TIMES_SKIP = 1000;


};

void InitSimulationParameters(SimulationParameters &data);
