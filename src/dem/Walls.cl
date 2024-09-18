
REAL4 GetNormalAndDistance(REAL4 TASKAS, REAL radius, REAL4 WALL_MIN, REAL4 WALL_MAX, int i) {

  if (i == 0)
    return MAKE_REAL4(-1, 0, 0, radius - fabs(WALL_MIN.x - TASKAS.x));

  if (i == 1)
    return MAKE_REAL4(1, 0, 0, radius - fabs(WALL_MAX.x - TASKAS.x));

  if (i == 2)
    return MAKE_REAL4(0, -1, 0, radius - fabs(WALL_MIN.y - TASKAS.y));

  if (i == 3)
    return MAKE_REAL4(0, 1, 0, radius - fabs(WALL_MAX.y - TASKAS.y));

  if (i == 4)
    return MAKE_REAL4(0, 0, -1, radius - fabs(WALL_MIN.z - TASKAS.z));

  if (i == 5)
    return MAKE_REAL4(0, 0, 1, radius - fabs(WALL_MAX.z - TASKAS.z));
  printf("Bad GetNormalAndDistance\n");
  return MAKE_REAL4(0, 0, 0, 0);
}

REAL4 GetNormalAndDistancePoint(REAL4 TASKAS, REAL radius, REAL4 WALL_MIN, REAL4 WALL_MAX, int i, REAL WALLL_RADIUS) {
  REAL4 dir = MAKE_REAL4(1, 1, 1, 0);
  REAL overlap = 1E6;
  if (i == 0) {
    dir = MAKE_REAL4(-1, 0, 0, 0);
    overlap = radius - fabs(WALL_MIN.x - TASKAS.x);
  }

  if (i == 1) {
    dir = MAKE_REAL4(1, 0, 0, 0);
    overlap = radius - fabs(WALL_MAX.x - TASKAS.x);
  }

  if (i == 2) {
    dir = MAKE_REAL4(0, -1, 0, 0);
    overlap = radius - fabs(WALL_MIN.y - TASKAS.y);
  }

  if (i == 3) {
    dir = MAKE_REAL4(0, 1, 0, 0);
    overlap = radius - fabs(WALL_MAX.y - TASKAS.y);
  }

  if (i == 4) {
    dir = MAKE_REAL4(0, 0, -1, 0);
    overlap = radius - fabs(WALL_MIN.z - TASKAS.z);
  }

  if (i == 5) {
    dir = MAKE_REAL4(0, 0, 1, 0);
    overlap = radius - fabs(WALL_MAX.z - TASKAS.z);
  }
  return TASKAS + dir * (radius + WALLL_RADIUS - overlap);
}
