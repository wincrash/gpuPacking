#include "Forces.h"
#include "Resource.h"
#include "core/Device.h"
#include "core/Parameters.h"
Forces::Forces(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string Forces::getModuleName() { return "Forces"; };

void Forces::Initialization() {
  auto result = Parameters::getInstance()->getResults();
  std::cout << "Start Initialization " << this->getModuleName() << "\n";
  sourceCL << "#define WORK_GROUP_SIZE " << WORK_GROUP_SIZE << "\n";
  sourceCL << "__constant INT wall_material=" << result["wall-material"].as<int>() << ";\n";
  Resource walls_cl = LOAD_RESOURCE(Walls_cl);
  sourceCL << std::string(walls_cl.data(), walls_cl.size()) << "\n";
  Resource models_cl = LOAD_RESOURCE(Models_cl);
  sourceCL << std::string(models_cl.data(), models_cl.size()) << "\n";

  Resource text = LOAD_RESOURCE(Forces_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";
  program = boost::compute::program::create_with_source(sourceCL.str(), boost::compute::system::default_context());
  program.build();
  KERNEL = boost::compute::kernel(program, "Forces");

  KERNEL.set_arg(0, data->POSITION_R);
  KERNEL.set_arg(1, data->VELOCITY_MASS);
  KERNEL.set_arg(2, data->ANGULAR_VELOCITY_MATERIAL);
  KERNEL.set_arg(3, data->FORCE);
  KERNEL.set_arg(4, data->TORQUE);
  KERNEL.set_arg(5, data->NN_COUNT);
  KERNEL.set_arg(6, data->NN_IDS);
  KERNEL.set_arg(7, data->FRICTION);
  KERNEL.set_arg(8, simParams->WALLS_MIN);
  KERNEL.set_arg(9, simParams->WALLS_MAX);
  KERNEL.set_arg(10, data->WALL_FORCES);

  std::cout << "Stop Initialization " << this->getModuleName() << "\n";
}

void Forces::Processing() {
  KERNEL.set_arg(8, simParams->WALLS_MIN);
  KERNEL.set_arg(9, simParams->WALLS_MAX);

  ENQUE_ND_JOB(KERNEL, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
}
