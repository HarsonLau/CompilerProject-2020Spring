#include "../run.h"
//A<16, 32>[i, j] = A<16, 32>[i, j] + alpha<1> * (B<16, 32>[i, k] * C<32, 32>[k, j]); A<16, 32>[i, j] = A<16, 32>[i, j] + beta<1> * D<16, 32>[i, j];

void kernel_case5(float (&B)[16][32], float (&C)[32][32], float (&D)[16][32], float (&alpha), float (&beta), float (&A)[16][32]) {
  for (int i=0; i<16; i++ ){
    for (int j=0; j<32; j++ ){
      for (int k=0; k<32; k++ ){
        A[i][j] =(A[i][j] + (alpha * (B[i][k] * C[k][j])));
      }
    }
  }
  for (int i=0; i<16; i++ ){
    for (int j=0; j<32; j++ ){
      A[i][j] =(A[i][j] + (beta * D[i][j]));
    }
  }
}
