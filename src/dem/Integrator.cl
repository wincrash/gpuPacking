
//__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1)))
__kernel void Integrator(
    REAL4_ARRAY POSITIONS_RADIUS,
    REAL4_ARRAY VELOCITY_MASS,
    REAL4_ARRAY FORCE,
    REAL4_ARRAY TORQUE,
    REAL4_ARRAY ANGULAR_VELOCITY_MATERIAL,
    INT_ARRAY FIX,
    unsigned long CSTEP) {

  INT idx = get_global_id(0);
  if (idx >= PARTICLE_COUNT)
    return;
  
  INT FIXAS = FIX[idx];
  REAL4 xx = POSITIONS_RADIUS[idx];
  REAL4 vv = VELOCITY_MASS[idx];
  REAL4 AV = ANGULAR_VELOCITY_MATERIAL[idx];
  REAL MAT_ID = AV.w;
  AV.w = 0;
  REAL4 T = TORQUE[idx];
  REAL4 F = FORCE[idx];

  REAL r = xx.w;
  const REAL mass = vv.w;

  xx.w = 0;
  vv.w = 0;

  F = F - PARALLEL_BOND_DAMPING * length(F)*normalize(vv);// * sign(vv);
  T = T - PARALLEL_BOND_DAMPING * length(T)*normalize(AV);// * sign(AV);

#ifdef ENABLE_FIX
  if (FIXAS > 0) {
    T = MAKE_REAL4_ZERO();
    F = MAKE_REAL4_ZERO();
    //F.y=0.0;
      vv.x=0;
  vv.z=0;
  }

    if(CSTEP>SPEED_STARTUP)
        CSTEP=SPEED_STARTUP;
     

  if (FIXAS == 1)  
    vv.y = FIX1.y*CSTEP/SPEED_STARTUP;

  if (FIXAS == 2)
    vv.y = FIX2.y*CSTEP/SPEED_STARTUP;
#endif
  /*

   xx = xx + vv * DELTA_TIME;
    
    xx.w=r;
    vv.w=mass;
    POSITIONS_RADIUS[idx]=xx;
    VELOCITY_MASS[idx]=vv;
  */


  xx=xx + vv * DELTA_TIME;
  vv = vv + DELTA_TIME * F / mass;

  vv.w = mass;
  xx.w = r;
  POSITIONS_RADIUS[idx] = xx;
  VELOCITY_MASS[idx] = vv ;

  REAL INERTIA = mass * r * r * 2.0 / 5.0;

  AV = AV + DELTA_TIME * T / INERTIA;

  AV.w = MAT_ID;

  ANGULAR_VELOCITY_MATERIAL[idx] = AV;
}
