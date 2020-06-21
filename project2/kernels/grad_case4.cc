#include "../run2.h"
//dC<32,32>[k,j]=dC<32,32>[k,j]+dA<16,32>[i,j]*B<16,32>[i,k];dB<16,32>[i,k]=dB<16,32>[i,k]+dA<16,32>[i,j]*C<32,32>[k,j];

void grad_case4(float (&B)[16][32], float (&C)[32][32], float (&dA)[16][32], float (&dB)[16][32], float (&dC)[32][32]) {
  for (int k=0; k<32; k++ ){
    for (int j=0; j<32; j++ ){
      for (int i=0; i<16; i++ ){
        dC[k][j] =(dC[k][j] + (dA[i][j] * B[i][k]));
      }
    }
  }
  for (int i=0; i<16; i++ ){
    for (int k=0; k<32; k++ ){
      for (int j=0; j<32; j++ ){
        B[i][k] =(dB[i][k] + (dA[i][j] * C[k][j]));
      }
    }
  }
}
