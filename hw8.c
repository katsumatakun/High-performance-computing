#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emmintrin.h>
#include <stdalign.h>

int main(int argc, char* argv[]){
  int a[1024];
  int b[1024];
  int c[1024];
  int d[1024];
  // float a_v[1024], b_v[1024],d_v[1024], c_v[1024], ones[1024], twos[1024];
  alignas(16) float a_v[1024], b_v[1024],d_v[1024], c_v[1024], ones[1024], twos[1024];
  for(int i=0; i<1024; i++){
    a[i] = i+1;
    b[i] = (i+1)*2-1;
    a_v[i] = i+1;
    b_v[i] = (i+1)*2-1;
    c[i] = (i+1)*2;
    c_v[i] = (i+1)*2-1;
    d[i] = 0;
    d_v[i] = 0;
    ones[i]=1;
    twos[i]=2;
  }

  for(int i=1; i<1023; i++){
    a[i] = b[i]+c[i];
    d[i] = a[i-1] + 1;
    b[i+1] = c[i] + 2;
  }

  printf("working\n");

  for(int i=1; i<1023; i+=4){

    printf("%d\n",i );
    __m128 vc = _mm_loadu_ps(&c_v[i]);
    // printf("aaaa\n");
    __m128 two = _mm_loadu_ps(&twos[i]);
    __m128 temp = _mm_add_ps(vc, two);
    _mm_storeu_ps(&b_v[i+1],temp);
    // b[i+1] = c[i] + 2;
    // printf("first line\n");
    __m128 vb = _mm_loadu_ps(&b_v[i]);
    __m128 va = _mm_add_ps(vb, vc);
    _mm_storeu_ps(&a_v[i], va);
    // a[i] = b[i]+c[i];

    // printf("second line\n");
    __m128 va_1 = _mm_loadu_ps(&a_v[i-1]);
    __m128 one = _mm_loadu_ps(&ones[i]);
    temp = _mm_add_ps(va_1,one);
    _mm_storeu_ps(&d_v[i], temp);
    // d[i] = a[i-1] + 1;

    // printf("third line\n");
  }
  printf("%f\n",d_v[1]);
  // for(int i=1; i<1023; i+=4){
  //
  //   printf("%d\n",i );
  //   __m128 vc = _mm_load_ps(c_v+i);
  //   printf("aaaa\n");
  //   __m128 two = _mm_load_ps(twos+i);
  //   __m128 temp = _mm_add_ps(vc, two);
  //   _mm_store_ps(b_v+i+1,temp);
  //   // b[i+1] = c[i] + 2;
  //   printf("first line\n");
  //   __m128 vb = _mm_load_ps(b_v+i);
  //   __m128 va = _mm_add_ps(vb, vc);
  //   _mm_store_ps(a_v+i, va);
  //   // a[i] = b[i]+c[i];
  //
  //   printf("second line\n");
  //   __m128 va_1 = _mm_load_ps(a_v+i-1);
  //   __m128 one = _mm_load_ps(ones+i);
  //   temp = _mm_add_ps(va_1,one);
  //   _mm_store_ps(d_v+i, temp);
  //   // d[i] = a[i-1] + 1;
  //
  //   printf("third line\n");
  // }




}
