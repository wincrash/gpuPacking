#include "ContactHistory.h"
#include "Resource.h"
#include "core/Device.h"

ContactHistory::ContactHistory(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string ContactHistory::getModuleName() { return "ContactHistory"; };

void ContactHistory::Initialization() {
  std::cout << "Start Initialization " << this->getModuleName() << "\n";
  sourceCL << "#define WORK_GROUP_SIZE " << WORK_GROUP_SIZE << "\n";
  Resource text = LOAD_RESOURCE(ContactHistory_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";
  program = boost::compute::program::create_with_source(sourceCL.str(), boost::compute::system::default_context());
  program.build();
  KERNEL = boost::compute::kernel(program, "ContactHistory");
  RESIZE(OLD_COUNT, data->PARTICLE_COUNT);
  RESIZE(OLD_IDS, data->PARTICLE_COUNT * data->NN_MAX);
  FILL(OLD_COUNT, 0);
  FILL(OLD_IDS, 0);
  WAIT();
  KERNEL.set_arg(0, OLD_COUNT);
  KERNEL.set_arg(1, OLD_IDS);
  KERNEL.set_arg(2, data->NN_COUNT);
  KERNEL.set_arg(3, data->NN_IDS);
  KERNEL.set_arg(4, data->FRICTION);
  std::cout << "Stop Initialization " << this->getModuleName() << "\n";
}

void ContactHistory::Processing() {
  if (!simParams->CONTACT_SEARCH)
    return;
  ENQUE_ND_JOB(KERNEL, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
  WAIT();
  COPY_VECTOR(data->NN_COUNT, OLD_COUNT);
  COPY_VECTOR(data->NN_IDS, OLD_IDS);
  WAIT();
}
