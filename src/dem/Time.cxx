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

}
