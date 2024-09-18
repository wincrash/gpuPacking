__attribute__((reqd_work_group_size(WORK_GROUP_SIZE, 1, 1))) __kernel void
ContactHistory(INT_ARRAY OLD_COUNT, INT_ARRAY OLD_IDS, INT_ARRAY NEW_COUNT,
               INT_ARRAY NEW_IDS, REAL4_ARRAY VALUES) {

  INT idx = get_global_id(0);
  INT lidx = get_local_id(0);
  __private REAL4 LOCAL_OLD_VALUES[NN_MAX];
  __local INT LOCAL_OLD_IDS[WORK_GROUP_SIZE][NN_MAX];
  if (idx >= PARTICLE_COUNT)
    return;

  INT OKiek = OLD_COUNT[idx];
  INT NKiek = NEW_COUNT[idx];
  INT offsetas = idx * NN_MAX;
  for (INT i = 0; i < OKiek; i++) {
    LOCAL_OLD_VALUES[i] = VALUES[offsetas + i];
    LOCAL_OLD_IDS[lidx][i] = OLD_IDS[offsetas + i];
  }

  for (INT i = 0; i < NKiek; i++) {
    INT id = NEW_IDS[idx * NN_MAX + i];
    REAL4 v = MAKE_REAL4_ZERO();
    for (INT k = 0; k < OKiek; k++) {
      if (id == LOCAL_OLD_IDS[lidx][k]) {
        v = LOCAL_OLD_VALUES[k];
        k = NN_MAX;
      }
    }
    VALUES[i] = v;
  }
}
