#pragma once
#include "core/AModule.h"

class Writer : public AModule {
public:
  Writer(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();

private:
  std::vector<REAL4> POSITION_R;
  std::vector<REAL4> VELOCITY;
  std::vector<INT> NN_COUNT;
  std::vector<INT> FIX;
  std::vector<INT> STOPPED;
  std::vector<REAL> MAX_OVERLAP;

};