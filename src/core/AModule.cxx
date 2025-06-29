#include "AModule.h"
#include "Parameters.h"
#include "Resource.h"


AModule::AModule(Data *data, SimulationParameters *simParams) {
  this->data = data;
  this->simParams = simParams;
  Resource text = LOAD_RESOURCE(DefaultOpenCLSource_cl);
  sourceCL << std::string(text.data(), text.size()) << "\n";

  sourceCL << "__constant REAL DELTA_TIME=" << simParams->DELTA << ";\n";
  sourceCL << "__constant REAL SKIN=" << simParams->SKIN << ";\n";
  sourceCL << "#define NN_MAX " << data->NN_MAX << "\n";
  sourceCL << "#define PARTICLE_COUNT " << data->PARTICLE_COUNT << "\n";
  sourceCL << "#define DELTA_TIME " << simParams->DELTA << "\n";
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