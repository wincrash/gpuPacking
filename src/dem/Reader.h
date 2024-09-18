#pragma once
#include "core/AModule.h"

class Reader : public AModule {
public:
  Reader(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();
};