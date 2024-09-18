#pragma once
#include "core/AModule.h"

class ContactSearch : public AModule {
public:
  ContactSearch(Data *data, SimulationParameters *simParams);
  virtual void Initialization();
  virtual std::string getModuleName();

protected:
  virtual void Processing();

private:
  INT_ARRAY CELL_ID;
  INT_ARRAY PARTICLE_ID;
  INT_ARRAY STARTAS;
  INT_ARRAY ENDAS;

  boost::compute::kernel HASH;
  boost::compute::kernel StartEnd;
  boost::compute::kernel STATIC_FILL;
};