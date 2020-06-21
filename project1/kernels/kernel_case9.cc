#include "../run.h"
//A<16, 32>[i, j] = A<16, 32>[i, j] + B<16, 32, 8>[i, k, l] * C<32, 32>[k, j] * D<8, 32>[l, j];

void kernel_case9(float (&B)[16][32][8], float (&C)[32][32], float (&D)[8][32], float (&A)[16][32]) {
  for (int i=0; i<16; i++ ){
    for (int j=0; j<32; j++ ){
      for (int k=0; k<32; k++ ){
        for (int l=0; l<8; l++ ){
          A[i][j] =(A[i][j] + ((B[i][k][l] * C[k][j]) * D[l][j]));
        }
      }
    }
  }
}
