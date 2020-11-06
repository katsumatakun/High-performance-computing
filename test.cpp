#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <sys/time.h>
#include <vector>
#include <algorithm>
#include <emmintrin.h>
#include <immintrin.h>
using namespace std;

int main()
{

  float* z = new float[21];
  float* x = new float[50];
  float* y = new float[50];
  for(int i = 0; i < 50; i++){
    x[i] =i%20;
    y[i] =i%20;
    z[i] = 2*i;
  }

  for(int i=0; i<50; i+=16){
   // __m512 vr = _mm512_load_ps(&x[i]);
    //__m512 vc = _mm512_load_ps(&y[i]);
    __m512i vr = _mm512_load_epi32(&y[i]);
    __m512i vc = _mm512_load_epi32(&x[i]);
    __m512 vinvec = _mm512_i32gather_ps(vc, z, 4);
    __m512 one = _mm512_set1_ps(1);
    __mmask16 mask = _mm512_cmpeq_ps_mask(one, vinvec);
    _mm512_mask_i32scatter_ps(x, mask, vc, one, 4);
    // cout << "vinvec" << endl;
  }
  return 0;
}
