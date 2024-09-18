#pragma once
#include "core/AModule.h"

class Integrator : public AModule {
public:
  Integrator(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();

private:
  boost::compute::kernel KERNEL;
};