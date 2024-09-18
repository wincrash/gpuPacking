#!/bin/bash
#SBATCH --job=SolidDEM
#SBATCH --nodes=1
#SBATCH --output=output.log

hostname;date;pwd
module load  compilers/gcc-11.3.0 compilers/cmake-3.27.0 devel/boost-1.82.0 devel/clinfo intel/compiler-2023.1.0 

/export/home/ruslan/SOFTWARE/gpuPacking/build/gpuPacking $(cat config.config)

hostname;date;pwd
