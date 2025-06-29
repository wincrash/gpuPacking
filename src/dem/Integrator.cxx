#include "Integrator.h"
#include "Resource.h"
#include "core/Device.h"
#include "core/Parameters.h"

Integrator::Integrator(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}

std::string Integrator::getModuleName() { return "Integrator"; };

void Integrator::Initialization() {

  auto result = Parameters::getInstance()->getResults();

  std::cout << "Start Initialization " << this->getModuleName() << "\n";
  Resource text = LOAD_RESOURCE(Integrator_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";
  program = boost::compute::program::create_with_source(sourceCL.str(), boost::compute::system::default_context());
  program.build();
  KERNEL = boost::compute::kernel(program, "Integrator");

  KERNEL.set_arg(0, data->POSITION_R);
  KERNEL.set_arg(1, data->VELOCITY);
  KERNEL.set_arg(2, data->FIX);
  KERNEL.set_arg(4, data->STOPPED);
  std::cout << "Stop Initialization " << this->getModuleName() << "\n";
}

void Integrator::Processing() { 
  KERNEL.set_arg(3,simParams->cstep);
  
  ENQUE_ND_JOB(KERNEL, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
   }
