#include "Time.h"
#include "core/Device.h"

Time::Time(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string Time::getModuleName() { return "Time"; };

void Time::Initialization() {
  simParams->COMPUTE = true;
  simParams->CONTACT_SEARCH = true;
  simParams->WRITE_RESULTS = true;
  simParams->PRINT_TIMES = true;
}

void Time::Processing() {
  simParams->ctime += simParams->DELTA;
  simParams->cstep++;
  simParams->PRINT_TIMES = (simParams->cstep % simParams->PRINT_TIMES_SKIP == 0);
  simParams->CONTACT_SEARCH = (simParams->cstep % simParams->CONTACT_SEARCH_SKIP == 0);
  simParams->WRITE_RESULTS = (simParams->cstep % simParams->WRITE_RESULTS_SKIP == 0);
  simParams->COMPUTE = (simParams->ctime <= simParams->END);

  //double KOEF=1.0*simParams->cstep/simParams->speed_startup;
  double KOEF=1.5707963267949*simParams->cstep/simParams->speed_startup;
  KOEF=std::sin(KOEF);
  if (simParams->cstep>=simParams->speed_startup) KOEF=1;


  simParams->WALLS_MIN[0] += simParams->WALL_SPEED[0]*KOEF * simParams->DELTA;
  simParams->WALLS_MAX[0] -= simParams->WALL_SPEED[1]*KOEF * simParams->DELTA;

  simParams->WALLS_MIN[1] += simParams->WALL_SPEED[2]*KOEF * simParams->DELTA;
  simParams->WALLS_MAX[1] -= simParams->WALL_SPEED[3]*KOEF * simParams->DELTA;

  simParams->WALLS_MIN[2] += simParams->WALL_SPEED[4]*KOEF * simParams->DELTA;
  simParams->WALLS_MAX[2] -= simParams->WALL_SPEED[5]*KOEF * simParams->DELTA;
}
