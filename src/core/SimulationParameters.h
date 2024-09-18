#pragma once
#include "core/DataTypes.h"

class SimulationParameters {
public:
  REAL AREA=1.0;
  REAL ctime = 0;
  ULONG cstep = 0;
  REAL END = 1.0;
  REAL DELTA = 1.E-5;
  std::vector<REAL> RO;
  std::vector<REAL> E;
  std::vector<REAL> NIU;
  std::vector<REAL> FRICTION;
  std::vector<INT> MAT_ID_1;
  std::vector<INT> MAT_ID_2;
  REAL NORMAL_DAMPING = 0;
  REAL TANGENTIAL_DAMPING = 0;
  REAL ROLLING_FRICTION = 0;

  REAL MIN_RADIUS = 0;
  REAL MAX_RADIUS = 0;
  REAL SKIN = 1.01;
  REAL4 BOUNDS_MIN = MAKE_REAL4(100000, 100000, 100000, 0);
  REAL4 BOUNDS_MAX = MAKE_REAL4(-100000, -100000, -100000, 0);
  REAL4 WALLS_MIN;
  REAL4 WALLS_MAX;
  std::vector<REAL> WALL_SPEED;


  std::vector<REAL> GRAVITATION;
  bool COMPUTE = true;
  bool CONTACT_SEARCH = true;
  bool WRITE_RESULTS = true;
  bool PRINT_TIMES = true;

  int CONTACT_SEARCH_SKIP = 10;
  int WRITE_RESULTS_SKIP = 1000;
  int PRINT_TIMES_SKIP = 1000;

  int speed_startup=1;
};

void InitSimulationParameters(SimulationParameters &data);
