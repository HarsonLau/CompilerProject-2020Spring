#include "../run2.h"
//dA<4,16>[i,k]=dA<4,16>[i,k]+dC<4,16>[i,j]*B<16,16>[k,j];

void grad_case3(float (&B)[16][16], float (&dC)[4][16], float (&dA)[4][16]) {
  for (int i=0; i<4; i++ ){
    for (int k=0; k<16; k++ ){
      for (int j=0; j<16; j++ ){
        dA[i][k] =(dA[i][k] + (dC[i][j] * B[k][j]));
      }
    }
  }
}
