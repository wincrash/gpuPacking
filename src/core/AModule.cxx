#include "AModule.h"
#include "Parameters.h"
#include "Resource.h"


AModule::AModule(Data *data, SimulationParameters *simParams) {
  this->data = data;
  this->simParams = simParams;
  Resource text = LOAD_RESOURCE(DefaultOpenCLSource_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";

  sourceCL << "__constant REAL DELTA_TIME=" << simParams->DELTA << ";\n";
  sourceCL << "__constant REAL NORMAL_DAMPING=" << simParams->NORMAL_DAMPING << ";\n";
  sourceCL << "__constant REAL TANGENTIAL_DAMPING=" << simParams->TANGENTIAL_DAMPING << ";\n";
  sourceCL << "__constant REAL ROLLING_FRICTION=" << simParams->ROLLING_FRICTION << ";\n";
  sourceCL << "__constant REAL SKIN=" << simParams->SKIN << ";\n";
  sourceCL << "#define NN_MAX " << data->NN_MAX << "\n";
  sourceCL << "#define PARTICLE_COUNT " << data->PARTICLE_COUNT << "\n";
  sourceCL << "#define BOND_COUNT " << data->BOND_COUNT << "\n";
  sourceCL << "#define DELTA_TIME " << simParams->DELTA << "\n";
  sourceCL << "#define SPEED_STARTUP "<<simParams->speed_startup<<"\n";


  sourceCL << RealArrayToString(simParams->E, "MAT_E");
  sourceCL << RealArrayToString(simParams->NIU, "MAT_NIU");
  sourceCL << RealArrayToString(simParams->RO, "RO");
  sourceCL << IntArrayToString(simParams->MAT_ID_1, "MAT_ID_1");
  sourceCL << IntArrayToString(simParams->MAT_ID_2, "MAT_ID_2");
  sourceCL << RealArrayToString(simParams->FRICTION, "FRICTION_COEF");

  sourceCL << "#define MAX_MATERIALS " << simParams->MAT_ID_2.size() << "\n";
  sourceCL << "__constant REAL4 GRAVITATION=MAKE_REAL4(" << simParams->GRAVITATION[0] << "," << simParams->GRAVITATION[1] << "," << simParams->GRAVITATION[2]
           << "," << 0 << ");\n";
}

void AModule::RunProcessing() {
  // std::cout<<"RunProcessing Start "<<this->getModuleName()<<"\n";
  moduleTimer.Start();
  this->Processing();
  moduleTimer.Stop();
  // std::cout<<"RunProcessing Stop "<<this->getModuleName()<<"\n";
}

double AModule::getModuleWorkTime() {
  moduleTimer.CalculateAVG();
  return moduleTimer.sumTime;
}

void AModule::printVector(std::string label, std::vector<int> &data) {
  for (int i = 0; i < data.size(); i++) {
    std::cout << data[i] << " ";
  }
  std::cout << std::endl;
}

std::string AModule::RealArrayToString(std::vector<REAL> data, std::string ArrayName) {
  std::stringstream str;
  str << "__constant REAL " << ArrayName << "[" << data.size() << "]={";
  for (INT i = 0; i < data.size(); i++) {
    str << GetDoubleFloatString(data[i]);
    if (i < data.size() - 1) {
      str << ",";
    } else {
      str << "};\n";
    }
  }
  return str.str();
}

std::string AModule::Real4ToString(std::vector<REAL> data, std::string ArrayName) {
  std::stringstream str;
  str << "__constant REAL4 " << ArrayName << "=MAKE_REAL4(" << data[0] << "," << data[1] << "," << data[2] << "," << data[3] << ");\n";
  return str.str();
}

std::string AModule::IntArrayToString(std::vector<INT> data, std::string ArrayName) {
  std::stringstream str;
  str << "__constant INT " << ArrayName << "[" << data.size() << "]={";
  for (INT i = 0; i < data.size(); i++) {
    str << data[i];
    if (i < data.size() - 1) {
      str << ",";
    } else {
      str << "};\n";
    }
  }
  return str.str();
}