#include "ContactSearch.h"
#include "Resource.h"
#include "core/Device.h"

ContactSearch::ContactSearch(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}

std::string ContactSearch::getModuleName() { return "ContactSearch"; };

void ContactSearch::Initialization() {
  std::cout << "Start Initialization " << this->getModuleName() << "\n";
  REAL CELL_SIZE = 2.0 * simParams->MAX_RADIUS * simParams->SKIN;
  REAL NX = (INT)(((simParams->BOUNDS_MAX[0] - simParams->BOUNDS_MIN[0]) / CELL_SIZE) + 1);
  REAL NY = (INT)(((simParams->BOUNDS_MAX[1] - simParams->BOUNDS_MIN[1]) / CELL_SIZE) + 1);
  REAL NZ = (INT)(((simParams->BOUNDS_MAX[2] - simParams->BOUNDS_MIN[2]) / CELL_SIZE) + 1);
  REAL NX_NY_NZ = NX * NY * NZ;

  sourceCL << "#define  NX " << NX << " \n";
  sourceCL << "#define  NY " << NY << " \n";
  sourceCL << "#define  NZ " << NZ << " \n";
  sourceCL << "#define  NX_NY_NZ " << NX_NY_NZ << " \n";
  sourceCL << "__constant  REAL CELL_SIZE=" << GetDoubleFloatString(CELL_SIZE) << ";\n";
  sourceCL << "__constant  REAL INV_CELL_SIZE=" << GetDoubleFloatString(1.0 / CELL_SIZE) << ";\n";
  sourceCL << "__constant  REAL4 GRID_ORIGIN=MAKE_REAL4(" << GetDoubleFloatString(simParams->BOUNDS_MIN[0]) << ","
           << GetDoubleFloatString(simParams->BOUNDS_MIN[1]) << "," << GetDoubleFloatString(simParams->BOUNDS_MIN[2]) << "," << GetDoubleFloatString(0.0)
           << ");\n";
  Resource walls_cl = LOAD_RESOURCE(Walls_cl);
  sourceCL << std::string(walls_cl.data(), walls_cl.size()) << "\n";
  Resource text = LOAD_RESOURCE(ContactSearch_cl);
  sourceCL << std::string(text.data(), text.size());

  program = boost::compute::program::create_with_source(sourceCL.str(), boost::compute::system::default_context());
  program.build();

  HASH = boost::compute::kernel(program, "CalculateHash");
  StartEnd = boost::compute::kernel(program, "StartEnd");
  STATIC_FILL = boost::compute::kernel(program, "STATIC_FILL");

  //////////////////////////
  RESIZE(STARTAS, NX_NY_NZ);
  RESIZE(ENDAS, NX_NY_NZ);

  RESIZE(CELL_ID, data->PARTICLE_COUNT);
  RESIZE(PARTICLE_ID, data->PARTICLE_COUNT);
  WAIT();

  HASH.set_arg(0, data->POSITION_R);
  HASH.set_arg(1, CELL_ID);
  HASH.set_arg(2, PARTICLE_ID);

  //////////////////////////
  StartEnd.set_arg(0, STARTAS);
  StartEnd.set_arg(1, ENDAS);
  StartEnd.set_arg(2, CELL_ID);

  STATIC_FILL.set_arg(0, STARTAS);
  STATIC_FILL.set_arg(1, ENDAS);
  STATIC_FILL.set_arg(2, CELL_ID);
  STATIC_FILL.set_arg(3, PARTICLE_ID);
  STATIC_FILL.set_arg(4, data->POSITION_R);
  STATIC_FILL.set_arg(5, data->NN_COUNT);
  STATIC_FILL.set_arg(6, data->NN_IDS);
  std::cout << "Stop Initialization " << this->getModuleName() << "\n";
}

void ContactSearch::Processing() {

  if (!simParams->CONTACT_SEARCH)
    return;

  FILL(data->NN_COUNT, 0);
  FILL(STARTAS, 0);
  FILL(ENDAS, -1);

  WAIT();

  ENQUE_ND_JOB(HASH, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
  WAIT();

  boost::compute::detail::radix_sort_by_key(CELL_ID.begin(), CELL_ID.end(), PARTICLE_ID.begin(), true, boost::compute::system::default_queue());
  WAIT();

  ENQUE_ND_JOB(StartEnd, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
  WAIT();
  STATIC_FILL.set_arg(7, simParams->WALLS_MIN);
  STATIC_FILL.set_arg(8, simParams->WALLS_MAX);
  ENQUE_ND_JOB(STATIC_FILL, data->PARTICLE_COUNT, WORK_GROUP_SIZE);
  WAIT();
}
