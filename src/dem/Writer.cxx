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
  POSITION_R0.resize(data->PARTICLE_COUNT);
  DISPLACEMENT.resize(data->PARTICLE_COUNT);
  POSITION_R.resize(data->PARTICLE_COUNT);
  VELOCITY_MASS.resize(data->PARTICLE_COUNT);
  ANGULAR_VELOCITY_MATERIAL.resize(data->PARTICLE_COUNT);
  FORCE.resize(data->PARTICLE_COUNT);
  TORQUE.resize(data->PARTICLE_COUNT);
  NN_COUNT.resize(data->PARTICLE_COUNT);
  FIX.resize(data->PARTICLE_COUNT);
  BONDS.resize(data->BOND_COUNT);
  BOND_STATE.resize(data->BOND_COUNT);
  BOND_FORCE_NORMAL.resize(data->BOND_COUNT);
  BOND_FORCE_TANGENTIAL.resize(data->BOND_COUNT);
  COPY_VECTOR(data->BONDS, BONDS);
  COPY_VECTOR(data->POSITION_R, POSITION_R0);
  COPY_VECTOR(data->FIX, FIX);
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

  COPY_VECTOR(data->POSITION_R, POSITION_R);
  COPY_VECTOR(data->VELOCITY_MASS, VELOCITY_MASS);
  COPY_VECTOR(data->ANGULAR_VELOCITY_MATERIAL, ANGULAR_VELOCITY_MATERIAL);
  COPY_VECTOR(data->FORCE, FORCE);
  COPY_VECTOR(data->TORQUE, TORQUE);
  COPY_VECTOR(data->NN_COUNT, NN_COUNT);
  COPY_VECTOR(data->BOND_STATE, BOND_STATE);
  COPY_VECTOR(data->FORCE_NORMAL, BOND_FORCE_NORMAL);
  COPY_VECTOR(data->FORCE_TANGENTIAL, BOND_FORCE_TANGENTIAL);
  WAIT();

  for (int i = 0; i < data->PARTICLE_COUNT; i++) {
    DISPLACEMENT[i][0] = POSITION_R[i][0] - POSITION_R0[i][0];
    DISPLACEMENT[i][1] = POSITION_R[i][1] - POSITION_R0[i][1];
    DISPLACEMENT[i][2] = POSITION_R[i][2] - POSITION_R0[i][2];
    DISPLACEMENT[i][3] = 0;
  }

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
  if (data->BOND_COUNT > 0) {
    file << "LINES " << data->BOND_COUNT << " " << data->BOND_COUNT * 3 << std::endl;
    for (size_t i = 0; i < data->BOND_COUNT; i++) {
      Write<int>(2, file);
      Write<int>(BONDS[i][0], file);
      Write<int>(BONDS[i][1], file);
    }
  } else {
    file << "VERTICES " << data->PARTICLE_COUNT << " " << data->PARTICLE_COUNT * 2 << std::endl;
    for (int i = 0; i < data->PARTICLE_COUNT; i++) {
      Write<int>(1, file);
      Write<int>(i, file);
    }
  }
  if (data->BOND_COUNT > 0) {
    WAIT();

    file << std::endl;
    file << "CELL_DATA " << data->BOND_COUNT << std::endl;
    file << "SCALARS STATUS int" << std::endl;
    file << "LOOKUP_TABLE default" << std::endl;
    for (size_t i = 0; i < data->BOND_COUNT; i++)
      Write<int>(BOND_STATE[i], file);
    file << std::endl;

    file << "SCALARS BOND_FORCE_NORMAL double" << std::endl;
    file << "LOOKUP_TABLE default" << std::endl;
    for (size_t i = 0; i < data->BOND_COUNT; i++)
      Write<double>(BOND_FORCE_NORMAL[i][3], file);
    file << std::endl;

    file << "SCALARS BOND_FORCE_TANGENTIAL double" << std::endl;
    file << "LOOKUP_TABLE default" << std::endl;
    for (size_t i = 0; i < data->BOND_COUNT; i++)
      Write<double>(BOND_FORCE_TANGENTIAL[i][3], file);
    file << std::endl;
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
    Write<double>(VELOCITY_MASS[i][0], file);
    Write<double>(VELOCITY_MASS[i][1], file);
    Write<double>(VELOCITY_MASS[i][2], file);
  }

  file << std::endl;

  file << "VECTORS ANGULAR_VELOCITY double" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) {
    Write<double>(ANGULAR_VELOCITY_MATERIAL[i][0], file);
    Write<double>(ANGULAR_VELOCITY_MATERIAL[i][1], file);
    Write<double>(ANGULAR_VELOCITY_MATERIAL[i][2], file);
  }

  file << std::endl;

  file << "VECTORS FORCE double" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) {
    Write<double>(FORCE[i][0], file);
    Write<double>(FORCE[i][1], file);
    Write<double>(FORCE[i][2], file);
  }
  file << std::endl;

  file << "VECTORS TORQUE double" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) {
    Write<double>(TORQUE[i][0], file);
    Write<double>(TORQUE[i][1], file);
    Write<double>(TORQUE[i][2], file);
  }
  file << std::endl;

  file << "VECTORS DISPLACEMENT double" << std::endl;
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) {
    Write<double>(POSITION_R[i][0] - POSITION_R0[i][0], file);
    Write<double>(POSITION_R[i][1] - POSITION_R0[i][1], file);
    Write<double>(POSITION_R[i][2] - POSITION_R0[i][2], file);
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

  // file << std::endl;

  file.close();
}
