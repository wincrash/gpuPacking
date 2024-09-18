#define NOT_BROKEN 0
#define BROKEN_NORMAL_TENSION 1
#define BROKEN_NORMAL_COMPRESSION 2
#define BROKEN_TANGENTIAL 3

__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1)))
__kernel void
TRANSFER(
    REAL4_ARRAY BOND_FORCE_NORMAL,
    REAL4_ARRAY BOND_FORCE_TANGENTIAL,
    REAL4_ARRAY PARTICLE_FORCE,
    INT_ARRAY COUNT,
    INT_ARRAY IDS,
    REAL4_ARRAY PARTICLE_TORQUE,
    REAL4_ARRAY BOND_MOMENT_N,
    REAL4_ARRAY BOND_MOMENT_S) {
  INT idx = get_global_id(0);
  INT lidx = get_local_id(0);

  if (idx < PARTICLE_COUNT) {
    INT kiekis = COUNT[idx];
    REAL4 F = PARTICLE_FORCE[idx];
    REAL4 TQ = PARTICLE_TORQUE[idx];
    for (INT i = 0; i < kiekis; i++) {
      INT id = IDS[idx * NN_MAX + i];

      if (id > 0) {
        INT indexas = id - 1;
        F = F - BOND_FORCE_NORMAL[indexas] - BOND_FORCE_TANGENTIAL[indexas];
        TQ = TQ - BOND_MOMENT_N[indexas] - BOND_MOMENT_S[indexas];
      } else {
        INT indexas = (-id) - 1;
        F = F + BOND_FORCE_NORMAL[indexas] + BOND_FORCE_TANGENTIAL[indexas];
        TQ = TQ + BOND_MOMENT_N[indexas] + BOND_MOMENT_S[indexas];
      }
    }
    F.w = 0;
    PARTICLE_FORCE[idx] = F;
    PARTICLE_TORQUE[idx] = TQ;
  }
}

__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1)))
__kernel void
BOND_FORCES(
    REAL4_ARRAY POSITION_R,
    REAL4_ARRAY VELOCITY_MASS,
    REAL4_ARRAY ANGULAR_VELOCITY_MATERIAL,
    INT_ARRAY BOND_STATE,
    INT2_ARRAY BONDS,
    REAL4_ARRAY FORCE_NORMAL,
    REAL4_ARRAY FORCE_TANGENTIAL,
    REAL4_ARRAY MOMENT_NORMAL,
    REAL4_ARRAY MOMENT_TANGENTIAL)

{
  INT idx = get_global_id(0);
  INT lidx = get_local_id(0);

  if (idx < BOND_COUNT) {

    if (BOND_STATE[idx] != NOT_BROKEN) // nutrukes
    {
      FORCE_NORMAL[idx] = MAKE_REAL4_ZERO();
      FORCE_TANGENTIAL[idx] = MAKE_REAL4_ZERO();
      MOMENT_NORMAL[idx] = MAKE_REAL4_ZERO();
      MOMENT_TANGENTIAL[idx] = MAKE_REAL4_ZERO();
      return;
    }

    INT B_TYPE = 0;
    INT2 BONDAS = BONDS[idx];

    REAL4 POSITION_1 = POSITION_R[BONDAS.x];
    REAL4 POSITION_2 = POSITION_R[BONDAS.y];

    REAL4 VELOCITY_1 = VELOCITY_MASS[BONDAS.x];
    REAL4 VELOCITY_2 = VELOCITY_MASS[BONDAS.y];

    REAL4 ANGULAR_VELOCITY_1 = ANGULAR_VELOCITY_MATERIAL[BONDAS.x];
    REAL4 ANGULAR_VELOCITY_2 = ANGULAR_VELOCITY_MATERIAL[BONDAS.y];

    INT MATERIAL_1 = ANGULAR_VELOCITY_1.w;
    INT MATERIAL_2 = ANGULAR_VELOCITY_2.w;
    ANGULAR_VELOCITY_1.w = 0;
    ANGULAR_VELOCITY_2.w = 0;
    REAL RADIUS_1 = POSITION_1.w;
    REAL RADIUS_2 = POSITION_2.w;
    POSITION_1.w = 0;
    POSITION_2.w = 0;

    VELOCITY_1.w = 0;
    VELOCITY_2.w = 0;

    ///
    REAL4 F_N = FORCE_NORMAL[idx];
    F_N.w = 0;
    REAL4 F_T = FORCE_TANGENTIAL[idx];
    F_T.w = 0;

    /////

    REAL4 MOMENT_N = MOMENT_NORMAL[idx];
    REAL4 MOMENT_S = MOMENT_TANGENTIAL[idx];

    REAL4 n_ij = POSITION_1 - POSITION_2;
    REAL h_ij = RADIUS_1 + RADIUS_2 - length(n_ij);
    n_ij = normalize(n_ij);

    //////////////////////////
    REAL r_ij = RADIUS_1 * RADIUS_2 / (RADIUS_1 + RADIUS_2);
    REAL4 d_cij = -(RADIUS_1 - 0.5 * h_ij) * n_ij;
    REAL4 d_cji = (RADIUS_2 - 0.5 * h_ij) * n_ij;
    REAL4 v_ij = VELOCITY_1 + cross(ANGULAR_VELOCITY_1, d_cij) - VELOCITY_2 - cross(ANGULAR_VELOCITY_2, d_cji);
    REAL KN = BOND_E[B_TYPE] / (RADIUS_1 + RADIUS_2);
    REAL R_MIN = BOND_RADIUS_MULTIPLIER[B_TYPE] * min(RADIUS_1, RADIUS_2);
    REAL A = PI * R_MIN * R_MIN;
    F_N = F_N + A * KN * (v_ij)*DELTA_TIME;
    // F_N=F_N+BOND_NORMAL_DAMPING*length(F_N)*normalize(v_ij);

    //////////////////////////

    REAL4 O_ij = (ANGULAR_VELOCITY_1 - ANGULAR_VELOCITY_2) * DELTA_TIME;
    REAL J = 0.5 * PI * R_MIN * R_MIN * R_MIN * R_MIN;
    REAL I = J * 0.5;

    MOMENT_N = MOMENT_N + (NORMAL_SHEAR_RATIO[B_TYPE] * KN) * J * O_ij;
    MOMENT_S = MOMENT_S + (KN)*I * O_ij;

    REAL4 v_n_ij = dot(v_ij, n_ij) * n_ij; // pagal statybos 15 formule
    REAL4 v_t_ij = v_ij - v_n_ij;          // pagal statybos 16 formule
    REAL4 v_ir_rt = v_t_ij * DELTA_TIME;
    F_T = F_T + (NORMAL_SHEAR_RATIO[B_TYPE] * KN) * A * v_ir_rt;

    REAL TENSILE = length(F_N);
    REAL SHEAR = length(F_T);
    REAL M_S = length(MOMENT_S);
    REAL M_N = length(MOMENT_N);
    F_N.w = TENSILE / A;
    F_T.w = SHEAR / A;
    F_N.w = F_N.w + length(MOMENT_S) * R_MIN / I;
    F_T.w = F_T.w + length(MOMENT_N) * R_MIN / J;
    /////////////////////

    // h_ij = INITIAL_DIJ[idx] - length(POSITION_1 - POSITION_2);

    // printf("hij %f\n",h_ij);
    /////////////////////

    if (F_N.w > (MAXIMAL_TENSILE_STRESS[B_TYPE]) && h_ij < 0) {
      BOND_STATE[idx] = BROKEN_NORMAL_TENSION;
      F_N = MAKE_REAL4_ZERO();
      F_T = MAKE_REAL4_ZERO();
    }

    if (F_N.w > (MAXIMAL_COMPRESSIVE_STRESS[B_TYPE]) && h_ij > 0) {
      BOND_STATE[idx] = BROKEN_NORMAL_COMPRESSION;
      F_N = MAKE_REAL4_ZERO();
      F_T = MAKE_REAL4_ZERO();
      MOMENT_N = MAKE_REAL4_ZERO();
      MOMENT_S = MAKE_REAL4_ZERO();
    }

    if (F_T.w > (MAXIMAL_SHEAR_STRESS[B_TYPE])) {
      BOND_STATE[idx] = BROKEN_TANGENTIAL;
      F_N = MAKE_REAL4_ZERO();
      F_T = MAKE_REAL4_ZERO();
      MOMENT_N = MAKE_REAL4_ZERO();
      MOMENT_S = MAKE_REAL4_ZERO();
    }

    FORCE_NORMAL[idx] = F_N;
    FORCE_TANGENTIAL[idx] = F_T;
    MOMENT_NORMAL[idx] = MOMENT_N;
    MOMENT_TANGENTIAL[idx] = MOMENT_S;
  }
}
