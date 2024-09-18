#pragma once
#include "Data.h"
#include "Device.h"
#include "SimulationParameters.h"
#include <boost/compute/program.hpp>
#include <boost/compute/utility/source.hpp>
#include <sstream>
#include <string>
#include <utils/Timer.h>
class AModule
{
public:
  AModule(Data *data, SimulationParameters *simParams);
  virtual void Initialization() = 0;
  void RunProcessing();
  virtual std::string getModuleName() = 0;
  double getModuleWorkTime();

protected:
  void printVector(std::string label, std::vector<int> &data);

  std::string RealArrayToString(std::vector<REAL> data, std::string ArrayName);
  std::string Real4ToString(std::vector<REAL> data, std::string ArrayName);
  std::string IntArrayToString(std::vector<INT> data, std::string ArrayName);

  virtual void Processing() = 0;
  boost::compute::program program;
  Data *data = nullptr;
  SimulationParameters *simParams = nullptr;
  std::stringstream sourceCL;
  int WORK_GROUP_SIZE = 32;

public:
  Timer moduleTimer;
};