#include "../run.h"
//A<16, 32>[i, j] = B<16, 32>[i, j] + C<16, 32>[i, j];

void kernel_case3(int (&B)[16][32], int (&C)[16][32], int (&A)[16][32]) {
  for (int i=0; i<16; i++ ){
    for (int j=0; j<32; j++ ){
      A[i][j] =(B[i][j] + C[i][j]);
    }
  }
}
