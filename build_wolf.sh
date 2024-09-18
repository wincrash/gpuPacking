#!/bin/bash

module load  compilers/gcc-11.3.0 compilers/cmake-3.27.0 devel/boost-1.82.0 devel/clinfo intel/compiler-2023.1.0 
cd build
cmake -DOpenCL_INCLUDE_DIR=/export/apps/devel/intel/oneapi/compiler/2023.1.0/linux/include/sycl  -DOpenCL_LIBRARY=/export/apps/devel/intel/oneapi/compiler/2023.1.0/linux/libOpenCL.so ..
make -j4
