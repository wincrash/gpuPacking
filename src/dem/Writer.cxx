#include "Writer.h"
#include "core/Device.h"
#include "utils/SystemCommand.h"
#include <fstream>
#include <sstream>

Writer::Writer(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string Writer::getModuleName() { return "Writer"; };

void Writer::Initialization() {
  SystemCommand("rm -rf data");
  SystemCommand("mkdir -p data");
  POSITION_R.resize(data->PARTICLE_COUNT);
  VELOCITY.resize(data->PARTICLE_COUNT);
  NN_COUNT.resize(data->PARTICLE_COUNT);
  FIX.resize(data->PARTICLE_COUNT);
  MAX_OVERLAP.resize(data->PARTICLE_COUNT);
  STOPPED.resize(data->PARTICLE_COUNT);

}
template <class T> void Write(T var, std::ofstream &file) {
  char *varArray = reinterpret_cast<char *>(&var);
  for (long i = 0; i < static_cast<long>(sizeof(var) / 2); i++)
    std::swap(varArray[sizeof(var) - 1 - i], varArray[i]);
  file.write(reinterpret_cast<char *>(&var), sizeof(T));
}

void Writer::Processing() {
  if (!this->simParams->WRITE_RESULTS)
    return;
  COPY_VECTOR(data->FIX, FIX);
  COPY_VECTOR(data->POSITION_R, POSITION_R);
  COPY_VECTOR(data->VELOCITY, VELOCITY);
  COPY_VECTOR(data->NN_COUNT, NN_COUNT);
  COPY_VECTOR(data->MAX_OVERLAP, MAX_OVERLAP);
  COPY_VECTOR(data->STOPPED, STOPPED);
  
  
  WAIT();

  std::stringstream str;
  str << "data/OUTPUT_" << std::setfill('0') << std::setw(10) << this->simParams->cstep << ".vtk";
  std::ofstream file(str.str(), std::ios::out | std::ios::binary);
  file.setf(std::ios_base::scientific, std::ios_base::floatfield);
  file << "# vtk DataFile Version 2.0" << std::endl;
  file << "VGTU-DEM\n"; //<< std::ctime(&current_time);
  file << "BINARY" << std::endl;
  file << "DATASET POLYDATA" << std::endl;
  file << std::endl;
  file << "POINTS " << data->PARTICLE_COUNT << " double" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) {
    Write<double>(POSITION_R[i][0], file);
    Write<double>(POSITION_R[i][1], file);
    Write<double>(POSITION_R[i][2], file);
  }

  file << std::endl;

    file << "VERTICES " << data->PARTICLE_COUNT << " " << data->PARTICLE_COUNT * 2 << std::endl;
    for (int i = 0; i < data->PARTICLE_COUNT; i++) {
      Write<int>(1, file);
      Write<int>(i, file);
    }
  

  file << std::endl;
  file << "POINT_DATA " << data->PARTICLE_COUNT << std::endl;

  file << "SCALARS RADIUS double 1" << std::endl;
  file << "LOOKUP_TABLE default" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    Write<double>(POSITION_R[i][3], file);

  file << std::endl;

  file << "VECTORS VELOCITY double" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) {
    Write<double>(VELOCITY[i][0], file);
    Write<double>(VELOCITY[i][1], file);
    Write<double>(VELOCITY[i][2], file);
  }

  file << std::endl;




  file << "SCALARS NN_COUNT int" << std::endl;
  file << "LOOKUP_TABLE default" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    Write<int>(NN_COUNT[i], file);
  file << std::endl;

  file << std::endl;

  file << "SCALARS FIX int" << std::endl;
  file << "LOOKUP_TABLE default" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    Write<int>(FIX[i], file);
  file << std::endl;

    file << std::endl;

  file << "SCALARS STOPPED int" << std::endl;
  file << "LOOKUP_TABLE default" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    Write<int>(STOPPED[i], file);
  file << std::endl;


file << std::endl;

  file << "SCALARS OVERLAP double" << std::endl;
  file << "LOOKUP_TABLE default" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    Write<double>(MAX_OVERLAP[i], file);
  file << std::endl;


  // file << std::endl;

  file.close();
}
