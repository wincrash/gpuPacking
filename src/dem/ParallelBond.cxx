#include "ParallelBond.h"
#include "Resource.h"
#include "core/Parameters.h"

ParallelBond::ParallelBond(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}

std::string ParallelBond::getModuleName() { return "ParallelBond"; };

void ParallelBond::Initialization() {
  auto results = Parameters::getInstance()->getResults();
  this->ENABLE_PARALLEL_BOND = results["enable-parallel-bond"].as<int>();
  if (this->ENABLE_PARALLEL_BOND == 0)
    return;

  sourceCL << "#define WORK_GROUP_SIZE " << WORK_GROUP_SIZE << "\n";
  std::vector<REAL> BOND_E = results["parallel-bond-E"].as<std::vector<double>>();
  std::vector<REAL> NORMAL_SHEAR_RATIO = results["parallel-bond-shear-ratio"].as<std::vector<double>>();
  std::vector<REAL> BOND_RADIUS_MULTIPLIER = results["parallel-bond-radius-multiplier"].as<std::vector<double>>();
  std::vector<REAL> MAXIMAL_TENSILE_STRESS = results["parallel-bond-max-tensile-stress"].as<std::vector<double>>();
  std::vector<REAL> MAXIMAL_SHEAR_STRESS = results["parallel-bond-max-shear-stress"].as<std::vector<double>>();
  std::vector<REAL> MAXIMAL_COMPRESSIVE_STRESS = results["parallel-bond-max-compression-stress"].as<std::vector<double>>();

  sourceCL << RealArrayToString(BOND_E, "BOND_E");
  sourceCL << RealArrayToString(NORMAL_SHEAR_RATIO, "NORMAL_SHEAR_RATIO");
  sourceCL << RealArrayToString(BOND_RADIUS_MULTIPLIER, "BOND_RADIUS_MULTIPLIER");
  sourceCL << RealArrayToString(MAXIMAL_TENSILE_STRESS, "MAXIMAL_TENSILE_STRESS");
  sourceCL << RealArrayToString(MAXIMAL_SHEAR_STRESS, "MAXIMAL_SHEAR_STRESS");
  sourceCL << RealArrayToString(MAXIMAL_COMPRESSIVE_STRESS, "MAXIMAL_COMPRESSIVE_STRESS");

  Resource text = LOAD_RESOURCE(ParallelBond_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";
  program = boost::compute::program::create_with_source(sourceCL.str(), boost::compute::system::default_context());
  program.build();

  FORCES = boost::compute::kernel(program, "BOND_FORCES");
  FORCES.set_arg(0, data->POSITION_R);
  FORCES.set_arg(1, data->VELOCITY_MASS);
  FORCES.set_arg(2, data->ANGULAR_VELOCITY_MATERIAL);
  FORCES.set_arg(3, data->BOND_STATE);
  FORCES.set_arg(4, data->BONDS);
  FORCES.set_arg(5, data->FORCE_NORMAL);
  FORCES.set_arg(6, data->FORCE_TANGENTIAL);
  FORCES.set_arg(7, data->MOMENT_NORMAL);
  FORCES.set_arg(8, data->MOMENT_TANGENTIAL);

  TRANSFER = boost::compute::kernel(program, "TRANSFER");
  TRANSFER.set_arg(0, data->FORCE_NORMAL);
  TRANSFER.set_arg(1, data->FORCE_TANGENTIAL);
  TRANSFER.set_arg(2, data->FORCE);
  TRANSFER.set_arg(3, data->BOND_TRANSFER_COUNT);
  TRANSFER.set_arg(4, data->BOND_TRANSFER_IDS);
  TRANSFER.set_arg(5, data->TORQUE);
  TRANSFER.set_arg(6, data->MOMENT_NORMAL);
  TRANSFER.set_arg(7, data->MOMENT_TANGENTIAL);
}

void ParallelBond::Processing() {
  if (this->ENABLE_PARALLEL_BOND == 0)
    return;
  ENQUE_ND_JOB(FORCES, data->BOND_COUNT, WORK_GROUP_SIZE);
  WAIT();
  ENQUE_ND_JOB(TRANSFER, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
  WAIT();
}
