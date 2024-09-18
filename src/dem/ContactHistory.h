#pragma once
#include "core/AModule.h"

class ContactHistory : public AModule {
public:
  ContactHistory(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();

private:
  boost::compute::kernel KERNEL;
  INT_ARRAY OLD_COUNT;
  INT_ARRAY OLD_IDS;
};