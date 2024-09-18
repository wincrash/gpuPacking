#include "Parameters.h"
/* Null, because instance will be initialized on demand. */
Parameters* Parameters::instance = nullptr;

Parameters* Parameters::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Parameters();
    }

    return instance;
}
Parameters::Parameters()
{

}
char **Parameters::copy_argv(int argc, char *argv[]) {
    // calculate the contiguous argv buffer size
    int length=0;
    size_t ptr_args = argc + 1;
    for (int i = 0; i < argc; i++)
    {
        length += (strlen(argv[i]) + 1);
    }
    char** new_argv = (char**)malloc((ptr_args) * sizeof(char*) + length);
    // copy argv into the contiguous buffer
    length = 0;
    for (int i = 0; i < argc; i++)
    {
        new_argv[i] = &(((char*)new_argv)[(ptr_args * sizeof(char*)) + length]);
        strcpy(new_argv[i], argv[i]);
        length += (strlen(argv[i]) + 1);
    }
    // insert NULL terminating ptr at the end of the ptr array
    new_argv[ptr_args-1] = NULL;
    return (new_argv);
}

void Parameters::InitArgs(int argc, char *argv[])
{
    this->argc=argc;
    this->argv=argv;
}



cxxopts::ParseResult Parameters::getResults()
{

    int argcc=this->argc;
    char** argvv=copy_argv(argcc,this->argv);
    try{
        cxxopts::Options options ("VGTU-DEM", "VGTU DEM software for heterogeneous materials");
        options.positional_help("[optional args]")
                .show_positional_help();
        options.add_options()
                ("help", "Print help")
                ;
        options.add_options("GENERAL")

                ("simulation-time","Time of total simulations",cxxopts::value<double>()->default_value("0.01"))
                ("mechanical-time-step","Time step for mechanical simulations",cxxopts::value<double>()->default_value("1E-5"))
                ("timer","Print computation times every N time steps",cxxopts::value<int>()->default_value("1000"))
                ("writer","Write computation results every N time steps",cxxopts::value<int>()->default_value("1000"))
                ("search","Write computation results every N time steps",cxxopts::value<int>()->default_value("10"))
                ("bounds","Bounds of the whole system xmin,xmax,ymin,ymax,zmin,zmax. Also it will place 6 walls on bounds",cxxopts::value<std::vector<double>>()->default_value("0,1,0,1,0,1"))
                ("wall-speeds","Define wall speeds, positive value only, 6 values for 6 wals. Xmin, Xmax, Ymin Ymax, Zmin Zmax",cxxopts::value<std::vector<double>>()->default_value("0,0,0,0,0,0"))
                ("load-output","Print wall loads every N steps",cxxopts::value<int>()->default_value("1000"))                
                ("fix1","Fixed particles speed and direction",cxxopts::value<std::vector<double>>()->default_value("0,0,0"))
                ("fix2","Fixed particles speed and direction",cxxopts::value<std::vector<double>>()->default_value("0,0,0"))
                ("enable-fix","Moving is based on fix. 1 - on , 0 off",cxxopts::value<int>()->default_value("0"))
                ("speed-startup","The steps count from which the speed of walls or fixed particles gets desire speed.",cxxopts::value<int>()->default_value("1"))
                ("area","Area for stress calculation. By default it takes average force of both walls/fix",cxxopts::value<double>()->default_value("1.0"))
                ;

        options.add_options("MATERIAL")
                ("gravitation","Gravitation of the system",cxxopts::value<std::vector<double>>()->default_value("0,0,0"))
                ("materials-E","Material normal stiffness",cxxopts::value<std::vector<double>>()->default_value("1E+7"))
                ("materials-NIU","Material tangential stiffness",cxxopts::value<std::vector<double>>()->default_value("0.2"))
                ("materials-RO","Material density",cxxopts::value<std::vector<double>>()->default_value("2400"))
                ("FRICTION","Friction coef",cxxopts::value<std::vector<double>>()->default_value("0.5"))
                ("ID1","Material combination ID1",cxxopts::value<std::vector<int>>()->default_value("0"))
                ("ID2","Material combination ID2",cxxopts::value<std::vector<int>>()->default_value("0"))

                ("normal-damping","Damping coeficient for normal force",cxxopts::value<double>()->default_value("0"))
                ("tangential-damping","Damping coeficient for tangential force",cxxopts::value<double>()->default_value("0"))
                ("rolling-friction","Rolling friction",cxxopts::value<double>()->default_value("0"))
                ("wall-material","wall material",cxxopts::value<int>()->default_value("0"))
                ;


        options.add_options("PARALLEL_BOND")
                ("enable-parallel-bond","Parallel bond enable 1 - on , 0 off",cxxopts::value<int>()->default_value("0"))
                ("parallel-bond-E","Young's modulus for parallel bond",cxxopts::value<std::vector<double>>()->default_value("1E+7"))
                ("parallel-bond-shear-ratio","Shear ratio for parallel bond",cxxopts::value<std::vector<double>>()->default_value("0.2"))
                ("parallel-bond-radius-multiplier","Radius multiplier for parallel bond",cxxopts::value<std::vector<double>>()->default_value("1.0"))
                ("parallel-bond-max-tensile-stress","Maximum tensile stress",cxxopts::value<std::vector<double>>()->default_value("1E+8"))
                ("parallel-bond-max-shear-stress","Maximum tensile stress",cxxopts::value<std::vector<double>>()->default_value("1E+7"))
                ("parallel-bond-max-compression-stress","Maximum compression stress",cxxopts::value<std::vector<double>>()->default_value("1E+15,1E+15,1E+15,1E+15,1E+15"))
                ("parallel-bond-damping","Histeric damping",cxxopts::value<double>()->default_value("0.0"))
                ;


        cxxopts::ParseResult result = (options.parse(argcc,argvv));
        if (result.count("help"))
        {
            std::cout << options.help({"","GENERAL","MATERIAL","PARALLEL_BOND","CONTACT_BOND"}) << std::endl;
            exit(0);
        }

        return result;
    }
    catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

}
