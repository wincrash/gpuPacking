inline INT Contact_Search_PP_UniformGrid_Hash(const INT x, const INT y, const INT z) {
  INT id = x + y * NX + z * NX * NY;
  if (id < 0 || id >= NX_NY_NZ) {
    id = 0;
  }
  return id;
}

/////////////////////////////////////////////
__kernel void CalculateHash(REAL4_ARRAY POSITIONS, INT_ARRAY CELL_ID, INT_ARRAY PARTICLE_ID) {
  INT idx = get_global_id(0);
  if (idx < PARTICLE_COUNT) {
    REAL4 p = (POSITIONS[idx] - GRID_ORIGIN) / CELL_SIZE;
    p = floor(p);
    CELL_ID[idx] = Contact_Search_PP_UniformGrid_Hash((INT)p.x, (INT)p.y, (INT)p.z);
    PARTICLE_ID[idx] = idx;
  }
}

////////////////////////////////////////////////////////

__kernel void StartEnd(INT_ARRAY cell_start, INT_ARRAY cell_end, INT_ARRAY CELL_ID) {

  INT idx = get_global_id(0);
  if (idx < PARTICLE_COUNT) {
    if (idx != 0) {
      INT bb = CELL_ID[idx - 1];
      INT cc = CELL_ID[idx];
      if (bb != cc) {
        cell_end[bb] = idx;
        cell_start[cc] = idx;
      }
      if (idx == (PARTICLE_COUNT - 1)) {
        cell_end[cc] = idx + 1;
      }
    }
  }
}

/////////////////////////////////////////////
__kernel void STATIC_FILL(INT_ARRAY cell_start, INT_ARRAY cell_end,
                          INT_ARRAY CELL_ID, INT_ARRAY PARTICLE_ID,
                          REAL4_ARRAY POSITION, INT_ARRAY NN_COUNT,
                          INT_ARRAY NN_IDS, REAL4 WALLS_MIN, REAL4 WALLS_MAX) {
  INT idx = get_global_id(0);
  if (idx < PARTICLE_COUNT) {

    REAL4 POINT = POSITION[idx];
    REAL radius = POINT.w;
    POINT.w = 0;
    REAL4 p = (POINT - GRID_ORIGIN) / CELL_SIZE; /// CELL_SIZE;
    p = floor(p);
    INT CX = (INT)p.x;
    INT CY = (INT)p.y;
    INT CZ = (INT)p.z;

    INT count = 0;
    for (INT i = CX - 1; i <= CX + 1; i++) {
      for (INT j = CY - 1; j <= CY + 1; j++) {
        for (INT k = CZ - 1; k <= CZ + 1; k++) {
          if (i < 0 || i >= NX || j < 0 || j >= NY || k < 0 || k >= NZ) {
            continue;
          }
          INT CELL_ID = Contact_Search_PP_UniformGrid_Hash(i, j, k);
          INT startas = cell_start[CELL_ID];
          INT endas = cell_end[CELL_ID];
          for (INT h = startas; h < endas; h++) {
            INT pid = PARTICLE_ID[h];
            if (pid == idx)
              continue;
            REAL4 P2 = POSITION[pid];
            REAL R2 = P2.w;
            P2.w = 0;
            REAL4 diff = P2 - POINT;
            REAL ilgis = radius * SKIN + R2 - length(diff);
            if (ilgis > -1.0E-8) {
              NN_IDS[idx * NN_MAX + count] = pid;
              count++;
            }
          }
        }
      }
    }
    for (int i = 0; i < 6; i++) {
      REAL4 n_ij = GetNormalAndDistance(POINT, radius, WALLS_MIN, WALLS_MAX, i);
      //////////////////////////
      if (n_ij.w > -radius * SKIN) {
        NN_IDS[idx * NN_MAX + count] = -(i + 1);
        count++;
      }
    }
    NN_COUNT[idx] = count;
  }
}
