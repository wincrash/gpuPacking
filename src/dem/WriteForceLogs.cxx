#include "WriteForceLogs.h"
#include "core/Device.h"
#include "core/Parameters.h"
#include "utils/SystemCommand.h"
#include <sstream>

WriteForceLogs::WriteForceLogs(Data *data, SimulationParameters *simParams) : AModule(data, simParams) {}
std::string WriteForceLogs::getModuleName() { return "WriteForceLogs"; };

void WriteForceLogs::Initialization()
{
  BOND_STATE.resize(data->BOND_COUNT);
  FORCE.resize(data->PARTICLE_COUNT);
  POSITION_R.resize(data->PARTICLE_COUNT);
  WALL_FORCES.resize(data->PARTICLE_COUNT);
  FIX.resize(data->PARTICLE_COUNT);
  COPY_VECTOR(data->FIX, FIX);
  auto result = Parameters::getInstance()->getResults();
  SKIP = result["load-output"].as<int>();
  std::ofstream file(filename);
  file.setf(std::ios_base::scientific, std::ios_base::floatfield);
  file << "TIME;STEP;WALL_STRAIN;WALL_STRESS;WALL_FORCE1;WALL_FORCE2;FIX_STRAIN;FIX_STRESS;FIX_FORCE1;FIX_FORCE2;NOT_BROKEN;BROKEN_NORMAL_TENSION;BROKEN_NORMAL_COMPRESSION;BROKEN_TANGENTIAL\n";
  file.close();

  for (int i = 0; i < data->PARTICLE_COUNT; i++)
  {
    if (FIX[i] == 1)
      fixas1.push_back(i);

    if (FIX[i] == 2)
      fixas2.push_back(i);
  }

  double miny=0;
  double maxy=0;
  COPY_VECTOR(data->POSITION_R, POSITION_R);
  for(int i=0;i<fixas1.size();i++)
  {
    miny=miny+POSITION_R[fixas1[i]][1];
  }

for(int i=0;i<fixas2.size();i++)
  {
    maxy=maxy+POSITION_R[fixas2[i]][1];
  }
  maxy=maxy/fixas2.size();
  miny=miny/fixas1.size();
  INIT_WALL_LENGTH = simParams->WALLS_MAX[1] - simParams->WALLS_MIN[1];
  INIT_FIX_LENGTH=fabs(maxy-miny);
}

void WriteForceLogs::Processing()
{
  if (simParams->cstep % SKIP != 0)
    return;

  double miny=0;
  double maxy=0;
  double FIX_F1=0;
  double FIX_F2=0;
 COPY_VECTOR(data->BOND_STATE, BOND_STATE);
  COPY_VECTOR(data->POSITION_R, POSITION_R);
  COPY_VECTOR(data->FORCE, FORCE);
  COPY_VECTOR(data->WALL_FORCES, WALL_FORCES);  
  for(int i=0;i<fixas1.size();i++)
  {
    miny=miny+POSITION_R[fixas1[i]][1];
    REAL4 f=FORCE[fixas1[i]];
    FIX_F1=FIX_F1+std::sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
  }

for(int i=0;i<fixas2.size();i++)
  {
    maxy=maxy+POSITION_R[fixas2[i]][1];
        REAL4 f=FORCE[fixas2[i]];
    FIX_F2=FIX_F2+std::sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
  }
  maxy=maxy/fixas2.size();
  miny=miny/fixas1.size();


  double W_STRAIN = 1.0 - (simParams->WALLS_MAX[1] - simParams->WALLS_MIN[1]) / INIT_WALL_LENGTH;
  double F_STRAIN=1-fabs(maxy-miny)/INIT_FIX_LENGTH;
  
 
  std::vector<int> bond_sates(4, 0);
  for (int i = 0; i < data->BOND_COUNT; i++)
  {
    bond_sates[BOND_STATE[i]]++;
  }


  std::vector<REAL> F(6, 0);
  for (int i = 0; i < data->PARTICLE_COUNT; i++)
  {
    REAL8 W = WALL_FORCES[i];
    for (int j = 0; j < 6; j++)
      F[j] += W[j];
  }

  std::ofstream file(filename, std::ios_base::app);
  file.setf(std::ios_base::scientific, std::ios_base::floatfield);
  file << simParams->ctime << ";";
  file << simParams->cstep << ";";

  file << W_STRAIN << ";";
  file << (F[2]+F[3])*0.5/simParams->AREA << ";";
  file << F[2] << ";";
  file << F[3] << ";";

    file << F_STRAIN << ";";
    file<<(FIX_F1+FIX_F2)*0.5/simParams->AREA<<";";
  file << FIX_F1 << ";";
  file << FIX_F2 << ";";

  file << bond_sates[0] << ";";
  file << bond_sates[1] << ";";
  file << bond_sates[2] << ";";
  file << bond_sates[3] << std::endl;

  file.close();
}
