#pragma once
#include "core/DataTypes.h"

class Data {
public:
  REAL4_ARRAY POSITION_R;
  REAL4_ARRAY VELOCITY;
  INT_ARRAY NN_COUNT;
  INT_ARRAY FIX;
  INT_ARRAY NN_IDS;
  REAL_ARRAY MAX_OVERLAP;

  INT_ARRAY STOPPED;

  INT PARTICLE_COUNT = 0;
  INT NN_MAX = 12;


};
