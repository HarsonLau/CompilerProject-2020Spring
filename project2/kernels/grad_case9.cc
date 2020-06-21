#include "../run2.h"
//dA<4>[i]=dA<4>[i]+dB<4,6>[i,j];

void grad_case9(float (&dB)[4][6], float (&dA)[4]) {
  for (int i=0; i<4; i++ ){
    for (int j=0; j<6; j++ ){
      dA[i] =(dA[i] + dB[i][j]);
    }
  }
}
