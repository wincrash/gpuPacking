cmake_minimum_required(VERSION 3.10)

project(gpuPack)

find_package(OpenCL)
include_directories(${OpenCL_INCLUDE_DIR})


FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})


#-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
SET(MY_GCC_COMPILE_FLAGS "-w")
add_definitions(${MY_GCC_COMPILE_FLAGS})
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MY_GCC_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${MY_GCC_COMPILE_FLAGS}" )


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS log )
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK=0)
include_directories(${Boost_INCLUDE_DIRS})


add_definitions(-DCL_TARGET_OPENCL_VERSION=220)
add_definitions(-DBOOST_COMPUTE_DEBUG_KERNEL_COMPILATION)

FILE(GLOB_RECURSE LibFiles "*.hpp")
add_custom_target(HEADERS SOURCES ${LibFiles})

set(CORE_FILES
    src/core/AModule.cxx
    src/core/AModule.h
    src/core/Data.cxx
    src/core/Data.h
    src/core/DataTypes.h
    src/core/Device.cxx
    src/core/Device.h
    src/core/SimulationParameters.cxx
    src/core/SimulationParameters.h
    src/core/Parameters.cxx
    src/core/Parameters.h
)

set(UTILS_FILES
    src/utils/SystemCommand.h
    src/utils/Timer.cxx
    src/utils/Timer.h
    src/utils/Logo.h
    src/utils/Logo.cxx

)

set(DEM_FILES
    src/dem/ContactSearch.cxx
    src/dem/ContactSearch.h
    src/dem/DefaultOpenCLSource.cl
    src/dem/Forces.cxx
    src/dem/Forces.h
    src/dem/Integrator.cxx
    src/dem/Integrator.h
    src/dem/Reader.cxx
    src/dem/Reader.h
    src/dem/Time.cxx
    src/dem/Time.h
    src/dem/Writer.cxx
    src/dem/Writer.h




)   
set(OPENCL_SOURCES
    src/dem/ContactSearch.cl
    src/dem/DefaultOpenCLSource.cl
    src/dem/Forces.cl
    src/dem/Integrator.cl
    src/dem/Models.cl
    src/dem/Walls.cl
)



include_directories(lib/embed-resource)
add_subdirectory(lib/embed-resource)
embed_resources(MyResources ${OPENCL_SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/src)


add_executable(${PROJECT_NAME} src/app/main.cxx ${MyResources} ${CORE_FILES} ${UTILS_FILES} ${DEM_FILES} ${OPENCL_SOURCES} )
target_link_libraries(${PROJECT_NAME}   -lpthread -lboost_log -lstdc++fs  -lOpenCL ${HDF5_LIBRARIES} ${VTK_LIBRARIES}  )


