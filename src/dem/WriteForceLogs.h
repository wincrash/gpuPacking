#pragma once
#include "core/AModule.h"

class WriteForceLogs : public AModule {
public:
  WriteForceLogs(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();

private:
  std::vector<REAL4> POSITION_R;
  std::vector<REAL4> FORCE;
  std::vector<INT> FIX;
  INT SKIP = 1;
  std::vector<REAL8> WALL_FORCES;
  std::vector<int> BOND_STATE;
  std::string filename = "LoadInfo.csv";


  double INIT_WALL_LENGTH=0;
  double INIT_FIX_LENGTH=0;
  std::vector<INT> fixas1;
  std::vector<INT> fixas2;
};