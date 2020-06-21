#include "../run2.h"
//dA<2,16>[i//16,i%16]=dA<2,16>[i//16,i%16]+dB<32>[i];

void grad_case8(float (&dB)[32], float (&dA)[2][16]) {
  for (int i=0; i<2; i++ ){
    if ((i/16<2)&&(i%16<16)&&(i/16<2)&&(i%16<16)) {
      dA[i/16][i%16] =(dA[i/16][i%16] + dB[i]);
    }
  }
}
