#pragma once
#include "core/DataTypes.h"
#include <boost/compute.hpp>
#include <boost/compute/algorithm.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/utility/dim.hpp>
#include <boost/lexical_cast.hpp>


inline std::string GetDoubleFloatString(REAL val) {
  return boost::lexical_cast<std::string>(val);
}

inline void WAIT() { boost::compute::system::default_queue().finish(); }

template <typename InputType, typename OutputType>
inline void COPY_VECTOR(InputType& input, OutputType& output) {
  boost::compute::copy(input.begin(), input.end(),output.begin());
  WAIT();
}

template <typename InputType, typename TypeVal>
inline void FILL(InputType& input, TypeVal val) {
  boost::compute::fill(input.begin(), input.end(),val);
  WAIT();
}

template <typename InputType>
inline void RESIZE(InputType& input, const INT N) {
  input = InputType(N);
  WAIT();
}

template <typename InputType>
inline void ENQUE_ND_JOB(InputType& kernel, const int N, const int WORK_GROUP_SIZE) {
  boost::compute::system::default_queue().enqueue_1d_range_kernel(kernel, 0, (std::ceil(N / WORK_GROUP_SIZE) + 1) * WORK_GROUP_SIZE, WORK_GROUP_SIZE).wait();
}