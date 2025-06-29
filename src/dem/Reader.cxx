#include "Reader.h"
#include "core/Device.h"
#include "core/Parameters.h"
#include <fstream>
Reader::Reader(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string Reader::getModuleName() { return "Reader"; };

void Reader::Initialization()
{

  auto results = Parameters::getInstance()->getResults();

  std::ifstream file("input.txt");
  if (!file.good())
    throw std::runtime_error("Error opening input.txt file!");
  double scale=results["scale"].as<double>();
  file >> data->PARTICLE_COUNT >> simParams->MIN_RADIUS >> simParams->MAX_RADIUS;
  simParams->MAX_RADIUS*=scale;
  std::cout<<data->PARTICLE_COUNT <<" "<< simParams->MIN_RADIUS <<" "<< simParams->MAX_RADIUS<<"\n";
  std::vector<REAL4> HOST_POSITION_R(data->PARTICLE_COUNT, MAKE_REAL4_ZERO());
  std::vector<INT> HOST_FIX(data->PARTICLE_COUNT, 0);
  for (int i = 0; i < data->PARTICLE_COUNT; i++)
  {
    int a;
    file >>a>> HOST_POSITION_R[i][0] >> HOST_POSITION_R[i][1] >> HOST_POSITION_R[i][2] >> HOST_POSITION_R[i][3] >> HOST_FIX[i];
    if(HOST_FIX[i]==0) HOST_POSITION_R[i][3]*=scale;
    
  }
  data->NN_MAX = (int)(4.0 * 0.74 * (simParams->MIN_RADIUS + simParams->MAX_RADIUS * simParams->SKIN) *
                       (simParams->MIN_RADIUS + simParams->MAX_RADIUS * simParams->SKIN) / (simParams->MIN_RADIUS * simParams->MIN_RADIUS)) +
                 1 + 4;

  std::cout << "Reader Particle Count " << data->PARTICLE_COUNT << "\n";
  std::cout << "Reader NN_MAX " << data->NN_MAX << "\n";
  std::cout << "Reader Particle minRadius " << simParams->MIN_RADIUS << "\n";
  std::cout << "Reader Particle maxRadius " << simParams->MAX_RADIUS << "\n";
  std::cout << "Reader Bounds min " << simParams->BOUNDS_MIN[0] << " " << simParams->BOUNDS_MIN[1] << " " << simParams->BOUNDS_MIN[2] << "\n";
  std::cout << "Reader Bounds max " << simParams->BOUNDS_MAX[0] << " " << simParams->BOUNDS_MAX[1] << " " << simParams->BOUNDS_MAX[2] << "\n";

  RESIZE(data->POSITION_R, data->PARTICLE_COUNT);
  RESIZE(data->VELOCITY, data->PARTICLE_COUNT);
  RESIZE(data->NN_COUNT, data->PARTICLE_COUNT);
  RESIZE(data->FIX, data->PARTICLE_COUNT);
  RESIZE(data->MAX_OVERLAP, data->PARTICLE_COUNT);
  RESIZE(data->STOPPED, data->PARTICLE_COUNT);
  
  
  RESIZE(data->NN_IDS, data->PARTICLE_COUNT * data->NN_MAX);
  COPY_VECTOR(HOST_POSITION_R, data->POSITION_R);
  COPY_VECTOR(HOST_FIX, data->FIX);

  FILL(data->VELOCITY, MAKE_REAL4_ZERO());
  FILL(data->NN_COUNT, 0);
  FILL(data->NN_IDS, 0);
  FILL(data->STOPPED, 0);
  FILL(data->MAX_OVERLAP, 0);
  

  REAL4 B_MIN = MAKE_REAL4(1E10, 1E10, 1E10, 0);
  REAL4 B_MAX = MAKE_REAL4(-1E10, -1E10, -1E10, 0);
  for (int i = 0; i < data->PARTICLE_COUNT; i++)
  {
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
  simParams->WALLS_MIN[0]=B_MIN[0];
  simParams->WALLS_MIN[1]=B_MIN[1];
  simParams->WALLS_MIN[2]=B_MIN[2];

  simParams->WALLS_MAX[0]=B_MAX[0];
  simParams->WALLS_MAX[1]=B_MAX[1];
  simParams->WALLS_MAX[2]=B_MAX[2];



    simParams->BOUNDS_MIN[0]=B_MIN[0];
  simParams->BOUNDS_MIN[1]=B_MIN[1];
  simParams->BOUNDS_MIN[2]=B_MIN[2];

  simParams->BOUNDS_MAX[0]=B_MAX[0];
  simParams->BOUNDS_MAX[1]=B_MAX[1];
  simParams->BOUNDS_MAX[2]=B_MAX[2];
}

void Reader::Processing() {}
