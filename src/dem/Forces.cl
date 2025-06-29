__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1)))
__kernel void
Forces(REAL4_ARRAY POSITION_R, REAL4_ARRAY VELOCITY,
       INT_ARRAY NN_COUNT, INT_ARRAY NN_IDS, REAL4 WALLS_MIN, REAL4 WALLS_MAX,REAL_ARRAY MAX_OVERLAP,INT_ARRAY STOPPED) {
//
  INT idx = get_global_id(0);
  if (idx >= PARTICLE_COUNT)
    return;
  REAL4 P1 = POSITION_R[idx];
  REAL4 V1 = VELOCITY[idx];
  REAL RADIUS_1 = P1.w;
  P1.w = 0;
  
  INT NN_KIEKIS = NN_COUNT[idx];
  //////////////////////////
  REAL4 P2 = MAKE_REAL4_ZERO();
  REAL RADIUS_2 = 1;
  REAL4 MY_VEL=MAKE_REAL4_ZERO();
  REAL maxOverlapas=0;

  //
  for (INT i = 0; i < NN_KIEKIS; i++) {
    INT FULL_OFFSET = idx * NN_MAX + i;
    INT pid = NN_IDS[idx * NN_MAX + i];
    int planeID = 0;
    if (pid < 0) {
      planeID = -(pid + 1);
      P2 = GetNormalAndDistancePoint(P1, RADIUS_1, WALLS_MIN, WALLS_MAX, planeID, RADIUS_2);
    } else {
      P2 = POSITION_R[pid];
      RADIUS_2 = P2.w;
      P2.w = 0;
    }

    REAL4 n_ij = P1 - P2; /// kryptis i dalele
    REAL h_ij = RADIUS_1 + RADIUS_2 - length(n_ij);
    n_ij = normalize(n_ij);
    if (h_ij < 0) {
      continue;
    }

  MY_VEL+=h_ij*n_ij;
  if(maxOverlapas<h_ij) maxOverlapas=h_ij;

  }
  //if(STOPPED[idx]==0)
  //{
   // if(maxOverlapas<1E-5) STOPPED[idx]=1;
 // }
  VELOCITY[idx]=MY_VEL;
  MAX_OVERLAP[idx]=maxOverlapas;
}
