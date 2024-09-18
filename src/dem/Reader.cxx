#include "Reader.h"
#include "core/Device.h"
#include "core/Parameters.h"
#include <fstream>
Reader::Reader(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string Reader::getModuleName() { return "Reader"; };

void Reader::Initialization() {

  auto results = Parameters::getInstance()->getResults();


  std::ifstream file("input.vtk");
  if (!file.good())
    throw std::runtime_error("Error opening input.vtk file!");
  std::string line;
  std::string text;

  std::getline(file, line); // file<<"# vtk DataFile Version 2.0"<<std::endl;
  std::getline(file, line); // file<<"ASCII"<<std::endl;
  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  file >> line >> data->PARTICLE_COUNT >> line;
  std::getline(file, line);
  std::vector<REAL4> HOST_POSITION_R(data->PARTICLE_COUNT, MAKE_REAL4_ZERO());
  std::vector<REAL4> HOST_VELOCITY_MASS(data->PARTICLE_COUNT, MAKE_REAL4_ZERO());
  std::vector<REAL4> HOST_ANGULAR_VELOCITY_MATERIAL(data->PARTICLE_COUNT, MAKE_REAL4_ZERO());
  std::vector<INT> HOST_FIX(data->PARTICLE_COUNT, 0);
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    file >> HOST_POSITION_R[i][0] >> HOST_POSITION_R[i][1] >> HOST_POSITION_R[i][2];



  std::getline(file, line);
  std::getline(file, line);
  // std::getline(file, line);
  std::string temp;
  file >> temp;
  file >> data->BOND_COUNT >> temp;

  std::vector<INT2> HOST_BONDS(data->BOND_COUNT);

  for (size_t i = 0; i < data->BOND_COUNT; i++) {
    INT2 b;
    file >> temp >> b[0] >> b[1];
    if (b[0] > b[1])
      std::swap(b[0], b[1]);
    HOST_BONDS[i] = b;
  }
  std::getline(file, line);

  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) 
    file >> HOST_POSITION_R[i][3];


  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++) 
    file >> HOST_ANGULAR_VELOCITY_MATERIAL[i][3];
  
  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    file >> HOST_FIX[i];

  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    file >> HOST_VELOCITY_MASS[i][0] >> HOST_VELOCITY_MASS[i][1] >> HOST_VELOCITY_MASS[i][2];

  std::getline(file, line);
  std::getline(file, line);
  std::getline(file, line);
  for (size_t i = 0; i < data->PARTICLE_COUNT; i++)
    file >> HOST_ANGULAR_VELOCITY_MATERIAL[i][0] >> HOST_ANGULAR_VELOCITY_MATERIAL[i][1] >> HOST_ANGULAR_VELOCITY_MATERIAL[i][2];


  simParams->MAX_RADIUS = -1000000000000000;
  simParams->MIN_RADIUS = 10000000000000000;

  for (int i = 0; i < data->PARTICLE_COUNT; i++) {
    double R = HOST_POSITION_R[i][3];
    if (simParams->MIN_RADIUS > R)
      simParams->MIN_RADIUS = R;
    if (simParams->MAX_RADIUS < R)
      simParams->MAX_RADIUS = R;
    int material = HOST_ANGULAR_VELOCITY_MATERIAL[i][3];
    double mass = (4.0 / 3.0) * PI_VALUE * R * R * R * simParams->RO[material];
    HOST_VELOCITY_MASS[i][3] = mass;
  }
  data->NN_MAX = (int)(4.0 * 0.74 * (simParams->MIN_RADIUS + simParams->MAX_RADIUS * simParams->SKIN) *
                       (simParams->MIN_RADIUS + simParams->MAX_RADIUS * simParams->SKIN) / (simParams->MIN_RADIUS * simParams->MIN_RADIUS)) +
                 1;

  std::cout << "Reader Particle Count " << data->PARTICLE_COUNT << "\n";
  std::cout << "Reader Bonds Count " << data->BOND_COUNT << "\n";
  std::cout << "Reader NN_MAX " << data->NN_MAX << "\n";
  std::cout << "Reader Particle minRadius " << simParams->MIN_RADIUS << "\n";
  std::cout << "Reader Particle maxRadius " << simParams->MAX_RADIUS << "\n";
  std::cout << "Reader Bounds min " << simParams->BOUNDS_MIN[0] << " " << simParams->BOUNDS_MIN[1] << " " << simParams->BOUNDS_MIN[2] << "\n";
  std::cout << "Reader Bounds max " << simParams->BOUNDS_MAX[0] << " " << simParams->BOUNDS_MAX[1] << " " << simParams->BOUNDS_MAX[2] << "\n";

  RESIZE(data->POSITION_R, data->PARTICLE_COUNT);
  RESIZE(data->VELOCITY_MASS, data->PARTICLE_COUNT);
  RESIZE(data->ANGULAR_VELOCITY_MATERIAL, data->PARTICLE_COUNT);
  RESIZE(data->FORCE, data->PARTICLE_COUNT);
  RESIZE(data->TORQUE, data->PARTICLE_COUNT);
  RESIZE(data->NN_COUNT, data->PARTICLE_COUNT);
  RESIZE(data->T1, data->PARTICLE_COUNT);
  RESIZE(data->T2, data->PARTICLE_COUNT);
  RESIZE(data->T3, data->PARTICLE_COUNT);
  RESIZE(data->FIX, data->PARTICLE_COUNT);
  RESIZE(data->WALL_FORCES, data->PARTICLE_COUNT);

  RESIZE(data->NN_IDS, data->PARTICLE_COUNT * data->NN_MAX);
  RESIZE(data->FRICTION, data->PARTICLE_COUNT * data->NN_MAX);

  COPY_VECTOR(HOST_POSITION_R, data->POSITION_R);
  COPY_VECTOR(HOST_VELOCITY_MASS, data->VELOCITY_MASS);
  COPY_VECTOR(HOST_ANGULAR_VELOCITY_MATERIAL, data->ANGULAR_VELOCITY_MATERIAL);
  COPY_VECTOR(HOST_FIX, data->FIX);

  FILL(data->FORCE, MAKE_REAL4_ZERO());
  FILL(data->TORQUE, MAKE_REAL4_ZERO());
  FILL(data->NN_COUNT, 0);
  FILL(data->T1, MAKE_REAL4_ZERO());
  FILL(data->T2, MAKE_REAL4_ZERO());
  FILL(data->T3, MAKE_REAL4_ZERO());
  FILL(data->NN_IDS, 0);
  FILL(data->FRICTION, MAKE_REAL4_ZERO());
  FILL(data->WALL_FORCES, MAKE_REAL8_ZERO());

  RESIZE(data->BONDS, data->BOND_COUNT);
  RESIZE(data->BOND_STATE, data->BOND_COUNT);

  RESIZE(data->FORCE_NORMAL, data->BOND_COUNT);
  RESIZE(data->FORCE_TANGENTIAL, data->BOND_COUNT);
  RESIZE(data->MOMENT_NORMAL, data->BOND_COUNT);
  RESIZE(data->MOMENT_TANGENTIAL, data->BOND_COUNT);

  FILL(data->FORCE_NORMAL, MAKE_REAL4_ZERO());
  FILL(data->FORCE_TANGENTIAL, MAKE_REAL4_ZERO());
  FILL(data->MOMENT_NORMAL, MAKE_REAL4_ZERO());
  FILL(data->MOMENT_TANGENTIAL, MAKE_REAL4_ZERO());

  FILL(data->BOND_STATE, 0);

  COPY_VECTOR(HOST_BONDS, data->BONDS);

  std::vector<int> HOST_COUNT;
  std::vector<int> HOST_IDS;
  HOST_IDS.resize(data->PARTICLE_COUNT * data->NN_MAX);
  HOST_COUNT.resize(data->PARTICLE_COUNT);
  RESIZE(data->BOND_TRANSFER_COUNT, data->PARTICLE_COUNT);
  RESIZE(data->BOND_TRANSFER_IDS, data->PARTICLE_COUNT * data->NN_MAX);
  WAIT();

  for (INT i = 0; i < data->BOND_COUNT; i++) {

    INT id1 = HOST_BONDS[i][0];
    INT id2 = HOST_BONDS[i][1];
    INT k1 = HOST_COUNT[id1];
    HOST_COUNT[id1]++;
    INT k2 = HOST_COUNT[id2];
    HOST_COUNT[id2]++;
    HOST_IDS[id1 * data->NN_MAX + k1] = i + 1;
    HOST_IDS[id2 * data->NN_MAX + k2] = -(i + 1);
  }
  COPY_VECTOR(HOST_COUNT, data->BOND_TRANSFER_COUNT);
  COPY_VECTOR(HOST_IDS, data->BOND_TRANSFER_IDS);
  WAIT();

  REAL4 B_MIN = MAKE_REAL4(1E10, 1E10, 1E10, 0);
  REAL4 B_MAX = MAKE_REAL4(-1E10, -1E10, -1E10, 0);
  for (int i = 0; i < data->PARTICLE_COUNT; i++) {
    REAL4 p = HOST_POSITION_R[i];
    if (B_MIN[0] > (p[0] - p[3]))
      B_MIN[0] = p[0] - p[3];
    if (B_MAX[0] < (p[0] + p[3]))
      B_MAX[0] = p[0] + p[3];

    if (B_MIN[1] > (p[1] - p[3]))
      B_MIN[1] = p[1] - p[3];
    if (B_MAX[1] < (p[1] + p[3]))
      B_MAX[1] = p[1] + p[3];

    if (B_MIN[2] > (p[2] - p[3]))
      B_MIN[2] = p[2] - p[3];
    if (B_MAX[2] < (p[2] + p[3]))
      B_MAX[2] = p[2] + p[3];
  }
}

void Reader::Processing() {}
