
//__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1)))
__kernel void Integrator(
    REAL4_ARRAY POSITIONS_RADIUS,
    REAL4_ARRAY VELOCITY,
    INT_ARRAY FIX,
    unsigned long CSTEP,
    INT_ARRAY STOPPED) {

  INT idx = get_global_id(0);
  if (idx >= PARTICLE_COUNT)
    return;
  
  INT FIXAS = FIX[idx];
  REAL4 xx = POSITIONS_RADIUS[idx];
  REAL4 vv = VELOCITY[idx];
  REAL r = xx.w;

  xx.w = 0;

  if (STOPPED[idx]>0)
  {
    vv=MAKE_REAL4_ZERO();
  }

  if (FIXAS>0){
    vv=MAKE_REAL4_ZERO();
  }

  xx=xx + vv * DELTA_TIME;

  xx.w = r;
  POSITIONS_RADIUS[idx] = xx;
  VELOCITY[idx] = vv ;

}
