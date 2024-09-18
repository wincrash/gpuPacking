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
  std::vector<REAL4> POSITION_R0;
  std::vector<REAL4> DISPLACEMENT;
  std::vector<REAL4> POSITION_R;
  std::vector<REAL4> VELOCITY_MASS;
  std::vector<REAL4> ANGULAR_VELOCITY_MATERIAL;
  std::vector<REAL4> FORCE;
  std::vector<REAL4> TORQUE;
  std::vector<INT> NN_COUNT;
  std::vector<INT> FIX;

  std::vector<INT2> BONDS;
  std::vector<INT> BOND_STATE;
  std::vector<REAL4> BOND_FORCE_NORMAL;
  std::vector<REAL4> BOND_FORCE_TANGENTIAL;
};