

int getMaterialCombination(int mat1, int mat2) {
  for (int i = 0; i < MAX_MATERIALS; i++) {
    if (mat1 == MAT_ID_1[i] && mat2 == MAT_ID_2[i])
      return i;
    if (mat1 == MAT_ID_2[i] && mat2 == MAT_ID_1[i])
      return i;
  }
  printf("Bad number of material. There is no combination for it.");
  return -1;
}
