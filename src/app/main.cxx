#include "core/Data.h"
#include "core/Device.h"
#include "core/Parameters.h"
#include "core/SimulationParameters.h"
#include "dem/ContactHistory.h"
#include "dem/ContactSearch.h"
#include "dem/Forces.h"
#include "dem/Integrator.h"
#include "dem/ParallelBond.h"
#include "dem/Reader.h"
#include "dem/Time.h"
#include "dem/WriteForceLogs.h"
#include "dem/Writer.h"
#include "utils/SystemCommand.h"
#include <sstream>
#include "utils/Logo.h"



int main(int argc, char *argv[]) {
  ShowLogo();
  Parameters::getInstance()->InitArgs(argc, argv);

  Data data;
  SimulationParameters simParams;
  InitSimulationParameters(simParams);

  Reader reader(&data, &simParams);
  reader.Initialization();

  std::vector<AModule *> modules;
  modules.push_back(new ContactSearch(&data, &simParams));
  modules.push_back(new ContactHistory(&data, &simParams));
  
  modules.push_back(new Forces(&data, &simParams));
  
  modules.push_back(new ParallelBond(&data, &simParams));
  modules.push_back(new Integrator(&data, &simParams));

  modules.push_back(new Time(&data, &simParams));
  modules.push_back(new WriteForceLogs(&data, &simParams));

  modules.push_back(new Writer(&data, &simParams));
  
  std::stringstream header;

  header << "TIME;STEP";
  for (int i = 0; i < modules.size(); i++) {
    modules[i]->Initialization();
    header << ";" << modules[i]->getModuleName();
  }
  header << ";Total\n";
  std::cout << header.str();
  std::ofstream file("timers.csv");
  file << header.str();
  file.close();
  

  modules[modules.size()-1]->RunProcessing();

  while (simParams.COMPUTE) {
    for (int i = 0; i < modules.size(); i++) {
      modules[i]->RunProcessing();
    }

    if (simParams.PRINT_TIMES) {
      std::stringstream line;
      line << simParams.ctime << ";";
      line << simParams.cstep << ";";
      double total = 0;
      for (int i = 0; i < modules.size(); i++) {
        double module_work_time = modules[i]->getModuleWorkTime();
        line << module_work_time << ";";
        total += module_work_time;
      }
      line << total << "\n";
      std::cout << line.str();
      std::ofstream file("timers.csv", std::ios_base::app);
      file << line.str();
      file.close();
    }
  }
  return 0;
}
