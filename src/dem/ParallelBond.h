#pragma once
#include "core/AModule.h"

class ParallelBond : public AModule {
public:
  ParallelBond(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();

private:
  boost::compute::kernel FORCES;
  boost::compute::kernel TRANSFER;
  INT ENABLE_PARALLEL_BOND=0;
};