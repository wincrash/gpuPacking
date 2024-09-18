__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1)))
__kernel void
Forces(REAL4_ARRAY POSITION_R, REAL4_ARRAY VELOCITY_MASS, REAL4_ARRAY ANGULAR_VELOCITY_MATERIAL, REAL4_ARRAY FORCE, REAL4_ARRAY TORQUE,
       INT_ARRAY NN_COUNT, INT_ARRAY NN_IDS, REAL4_ARRAY FRICTION, REAL4 WALLS_MIN, REAL4 WALLS_MAX, REAL8_ARRAY WALL_FORCE) {
//
  INT idx = get_global_id(0);
  if (idx >= PARTICLE_COUNT)
    return;
  REAL4 P1 = POSITION_R[idx];
  REAL4 V1 = VELOCITY_MASS[idx];
  REAL4 AV1 = ANGULAR_VELOCITY_MATERIAL[idx];
  REAL RADIUS_1 = P1.w;
  P1.w = 0;
  REAL MASS_1 = V1.w;
  V1.w = 0;
  INT MAT_ID_1 = (int)AV1.w;
  AV1.w = 0;
  REAL4 F = GRAVITATION * MASS_1;
  REAL4 T = MAKE_REAL4_ZERO();
  REAL8 WFORCE = MAKE_REAL8_ZERO();

  INT NN_KIEKIS = NN_COUNT[idx];
  //////////////////////////
  REAL4 P2 = MAKE_REAL4_ZERO();
  REAL4 V2 = MAKE_REAL4_ZERO();
  REAL4 AV2 = MAKE_REAL4_ZERO();
  REAL RADIUS_2 = 1;
  REAL MASS_2 = 1;
  INT MAT_ID_2 = 0;

  //
  for (INT i = 0; i < NN_KIEKIS; i++) {
    INT FULL_OFFSET = idx * NN_MAX + i;
    INT pid = NN_IDS[idx * NN_MAX + i];
    int planeID = 0;
    if (pid < 0) {
      RADIUS_2 = 1E6;
      MASS_2 = 1E6;
      V2 = MAKE_REAL4_ZERO();
      AV2 = MAKE_REAL4_ZERO();
      planeID = -(pid + 1);
      P2 = GetNormalAndDistancePoint(P1, RADIUS_1, WALLS_MIN, WALLS_MAX, planeID, RADIUS_2);
      MAT_ID_2 = wall_material;
    } else {
      P2 = POSITION_R[pid];
      V2 = VELOCITY_MASS[pid];
      AV2 = ANGULAR_VELOCITY_MATERIAL[pid];
      RADIUS_2 = P2.w;
      P2.w = 0;
      MASS_2 = V2.w;
      V2.w = 0;
      MAT_ID_2 = (int)AV2.w;
      AV2.w = 0;
    }

    REAL m_ij = MASS_1 * MASS_2 / (MASS_1 + MASS_2);
    REAL r_ij = RADIUS_1 * RADIUS_2 / (RADIUS_1 + RADIUS_2);
    REAL4 n_ij = P1 - P2; /// kryptis i dalele
    REAL h_ij = RADIUS_1 + RADIUS_2 - length(n_ij);
    // REAL h_ij = length(n_ij) - RADIUS_1 - RADIUS_2;
    n_ij = normalize(n_ij);
    // n_ij=-n_ij;

    INT MATID = getMaterialCombination(MAT_ID_1, MAT_ID_2);
    //////////////////////////
    if (h_ij < 0) {
      FRICTION[FULL_OFFSET] = MAKE_REAL4_ZERO();
      continue;
    }

    REAL4 d_cij = -(RADIUS_1 - 0.5 * h_ij) * n_ij;
    REAL4 d_cji = -(RADIUS_2 - 0.5 * h_ij) * (-n_ij);
    REAL4 v_ij = V1 + cross(AV1, d_cij) - (V2 + cross(AV2, d_cji));
    REAL4 v_n_ij = dot(v_ij, n_ij) * n_ij;
    REAL4 v_t_ij = v_ij - v_n_ij;

    REAL KN1=4.0*MAT_E[MAT_ID_1]*RADIUS_1;
    REAL KN2=4.0*MAT_E[MAT_ID_2]*RADIUS_2;
    REAL KT1=KN1*MAT_NIU[MAT_ID_1];
    REAL KT2=KN2*MAT_NIU[MAT_ID_2];
    REAL KT = KT1*KT2/(KT1+KT2);  

    REAL KN = KN1*KN2/(KN1+KN2);
    REAL4 F_N = KN * n_ij * h_ij;
    //REAL4 F_N_DISS=2.0*log(NORMAL_DAMPING)/sqrt(log(NORMAL_DAMPING)*log(NORMAL_DAMPING)+PI*PI)*sqrt(m_ij*KN)*v_n_ij;
    //F_N+=F_N_DISS;



// REAL KN = MAT_KN[MATID] * sqrt(r_ij);
// REAL4   F_N = n_ij * (KN * pow(h_ij, 1.5));
 //   F_N = F_N - (-sqrt(5.0) * log(NORMAL_DAMPING) / sqrt(log(NORMAL_DAMPING) * log(NORMAL_DAMPING) + PI * PI)) *
                    pow(2.0 * m_ij, 0.5) * pow(KN, 0.5) * pow(h_ij, 0.25) *
                    v_n_ij;



    REAL4 ShearSlip = FRICTION[FULL_OFFSET] + v_t_ij * DELTA_TIME;
    FRICTION[FULL_OFFSET] = ShearSlip;
    REAL shear_slipas_length = length(ShearSlip);
    REAL4 t_ij = MAKE_REAL4_ZERO();
    if (length(v_t_ij) > 0) {
      t_ij = normalize(v_t_ij);
    } else {
      if (shear_slipas_length > 0) {
        t_ij = normalize(ShearSlip);
      } else {
        t_ij = MAKE_REAL4_ZERO();
      }
    }
    REAL4 F_t_ij_dyn =- length(F_N) * FRICTION_COEF[MATID] * t_ij;                                                    // formule 24
    REAL4 F_T = -KT * sqrt(r_ij * h_ij) * shear_slipas_length * t_ij - TANGENTIAL_DAMPING * m_ij * v_t_ij; // formule 25
                                                                                                                     // REAL4 F_T=MAT_KT[MATID]*r_ij*shear_slipas_length*t_ij;
    if (length(F_T) > length(F_t_ij_dyn)) {
      F_T = F_t_ij_dyn;
    }

    T += cross(d_cij, F_T + F_N);
    
    T -= ROLLING_FRICTION * r_ij * length(F_N) * normalize(AV1 - AV2);
    F += (F_N + F_T);

    if (pid < 0) {
      REAL f_total = length(F_N + F_T);
      if (planeID == 0)
        WFORCE.s0 = f_total;
      if (planeID == 1)
        WFORCE.s1 = f_total;
      if (planeID == 2)
        WFORCE.s2 = f_total;
      if (planeID == 3)
        WFORCE.s3 = f_total;
      if (planeID == 4)
        WFORCE.s4 = f_total;
      if (planeID == 5)
        WFORCE.s5 = f_total;
    }
  }
  FORCE[idx] = F;
  TORQUE[idx] = T;
  WALL_FORCE[idx] = WFORCE;
}
