#include "Integrator.h"
#include "Resource.h"
#include "core/Device.h"
#include "core/Parameters.h"

Integrator::Integrator(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}

std::string Integrator::getModuleName() { return "Integrator"; };

void Integrator::Initialization() {

  auto result = Parameters::getInstance()->getResults();
  std::vector<double> fix1 = result["fix1"].as<std::vector<double>>();
  std::vector<double> fix2 = result["fix2"].as<std::vector<double>>();
  int enableFix = result["enable-fix"].as<int>();
  if (enableFix == 1)
    sourceCL << "#define ENABLE_FIX 1\n";
  sourceCL << "__constant REAL4 FIX1=MAKE_REAL4(" << fix1[0] << "," << fix1[1] << "," << fix1[2] << "," << 0 << ");\n";
  sourceCL << "__constant REAL4 FIX2=MAKE_REAL4(" << fix2[0] << "," << fix2[1] << "," << fix2[2] << "," << 0 << ");\n";
  sourceCL << "const __constant REAL PARALLEL_BOND_DAMPING=" << result["parallel-bond-damping"].as<double>() << ";\n";

  std::cout << "Start Initialization " << this->getModuleName() << "\n";
  Resource text = LOAD_RESOURCE(Integrator_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";
  program = boost::compute::program::create_with_source(sourceCL.str(), boost::compute::system::default_context());
  program.build();
  KERNEL = boost::compute::kernel(program, "Integrator");

  KERNEL.set_arg(0, data->POSITION_R);
  KERNEL.set_arg(1, data->VELOCITY_MASS);
  KERNEL.set_arg(2, data->FORCE);
  KERNEL.set_arg(3, data->TORQUE);
  KERNEL.set_arg(4, data->ANGULAR_VELOCITY_MATERIAL);
  KERNEL.set_arg(5, data->FIX);
  std::cout << "Stop Initialization " << this->getModuleName() << "\n";
}

void Integrator::Processing() { 
  KERNEL.set_arg(6,simParams->cstep);
  
  ENQUE_ND_JOB(KERNEL, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
   }
