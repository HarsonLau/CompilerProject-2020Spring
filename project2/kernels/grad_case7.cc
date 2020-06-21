#include "../run2.h"
//dA<32,16>[j,i]=dA<32,16>[j,i]+dB<16,32>[i,j];

void grad_case7(float (&dB)[16][32], float (&dA)[32][16]) {
  for (int j=0; j<32; j++ ){
    for (int i=0; i<16; i++ ){
      dA[j][i] =(dA[j][i] + dB[i][j]);
    }
  }
}
