#pragma once
#include "core/AModule.h"

class Time : public AModule {
public:
  Time(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();
};