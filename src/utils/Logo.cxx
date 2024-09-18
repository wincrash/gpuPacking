#include "Logo.h"

#include <string>
#include <iostream>
#include <boost/compute/system.hpp>
void ShowLogo() {
    std::cout<<
                "\n\n\n"


"███████╗ ██████╗ ██╗     ██╗██████╗ ██████╗ ███████╗███╗   ███╗\n"
"██╔════╝██╔═══██╗██║     ██║██╔══██╗██╔══██╗██╔════╝████╗ ████║\n"
"███████╗██║   ██║██║     ██║██║  ██║██║  ██║█████╗  ██╔████╔██║\n"
"╚════██║██║   ██║██║     ██║██║  ██║██║  ██║██╔══╝  ██║╚██╔╝██║\n"
"███████║╚██████╔╝███████╗██║██████╔╝██████╔╝███████╗██║ ╚═╝ ██║\n"
"╚══════╝ ╚═════╝ ╚══════╝╚═╝╚═════╝ ╚═════╝ ╚══════╝╚═╝     ╚═╝\n"
                "\n\n"
                "                         Author: RUSLAN PACEVIC            "
                "                 \n"
                "                         E-mail: ruslan.pacevic@vilniustech.lt    "
                "                 \n"
                "                    Institution: Vilnius Gediminas "
                "Technical university     \n"
                "\n\n";
    std::cout<<"Enviroment variable  BOOST_COMPUTE_DEFAULT_DEVICE - name of the compute device (e.g. GTX TITAN)"<<std::endl;
    std::cout<<"Enviroment variable  BOOST_COMPUTE_DEFAULT_DEVICE_TYPE - type of the compute device (e.g. GPU or CPU)"<<std::endl;
    std::cout<<"Enviroment variable  BOOST_COMPUTE_DEFAULT_PLATFORM - name of the platform (e.g. NVIDIA CUDA)"<<std::endl;
    std::cout<<"Enviroment variable  BOOST_COMPUTE_DEFAULT_VENDOR - name of the device vendor (e.g. NVIDIA)"<<std::endl;

    std::cout<<"Count : "<<boost::compute::system::device_count()<<std::endl;


    std::vector<boost::compute::platform> platforms =
            boost::compute::system::platforms();
    for (size_t i = 0; i < platforms.size(); i++) {
        const boost::compute::platform &platform = platforms[i];
        std::cout<<"Platform : "<<platform.name()<<std::endl;

        std::vector<boost::compute::device> devices = platform.devices();
        for (size_t j = 0; j < devices.size(); j++) {
            const boost::compute::device &device = devices[j];

            std::string type;
            if (device.type() & boost::compute::device::gpu)
                type = "GPU Device";
            else if (device.type() & boost::compute::device::cpu)
                type = "CPU Device";
            else if (device.type() & boost::compute::device::accelerator)
                type = "Accelerator Device";
            else
                type = "Unknown Device";

            std::cout<<"type : "<<device.name()<<std::endl;

        }
    }
    std::cout<<"Using device : "<<boost::compute::system::default_device().name()<<std::endl;
}
