#include "../run2.h"
//dB<2,16,7,7>[n,c,p+r,q+s]=dB<2,16,7,7>[n,c,p+r,q+s]+dA<2,8,5,5>[n,k,p,q]*C<8,16,3,3>[k,c,r,s];

void grad_case6(float (&C)[8][16][3][3], float (&dA)[2][8][5][5], float (&dB)[2][16][7][7]) {
  for (int n=0; n<2; n++ ){
    for (int c=0; c<16; c++ ){
      for (int p=0; p<5; p++ ){
        for (int r=0; r<3; r++ ){
          for (int q=0; q<5; q++ ){
            for (int s=0; s<3; s++ ){
              for (int k=0; k<8; k++ ){
                if ((p+r<7)&&(q+s<7)&&(p+r<7)&&(q+s<7)) {
                  dB[n][c][(p + r)][(q + s)] =(dB[n][c][(p + r)][(q + s)] + (dA[n][k][p][q] * C[k][c][r][s]));
                }
              }
            }
          }
        }
      }
    }
  }
}
