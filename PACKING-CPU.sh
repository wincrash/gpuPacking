#!/bin/bash
#SBATCH --job=gpuPacking
#SBATCH --nodes=1
#SBATCH --output=output.log

hostname;date;pwd
module load  compilers/gcc-11.3.0 compilers/cmake-3.27.0 devel/boost-1.82.0 devel/clinfo intel/compiler-2023.1.0 
export BOOST_COMPUTE_DEFAULT_DEVICE_TYPE=CPU
/export/home/ruslan/SOFTWARE/gpuPacking/build/gpuPack $(cat config.config)

hostname;date;pwd
