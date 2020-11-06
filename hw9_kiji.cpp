#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <sys/time.h>
#include <vector>
#include <algorithm>
#include <immintrin.h>
using namespace std;

int main()
 {

  // 456631 456631 14855875
  ifstream fin("higgs-twitter.mtx");
  // ifstream fin("matrix.txt");
  // Read number of rows, number of columns, and number of nonzeros from the file
  int nr, nc, nnz;
  fin >> nr >> nc >> nnz;
  cout << "nrows: " << nr << " ncols: " << nc << " nnz: " << nnz << endl;

  int tile_size = 2048;
  int tile_num = ((nr+tile_size-1)/tile_size);

  cout << "tile" << tile_num << endl;

  vector<float> *row_vec = new vector<float>[tile_num*tile_num];
  vector<float> *col_vec = new vector<float>[tile_num*tile_num];
  vector<float> *weight_vec = new vector<float>[tile_num*tile_num];

  // new __mmmask16[num of total including padding and nonzero/16]

  int count = 0;
  int r, c, w;
  while (fin >> r >> c >> w) {
    int tile = c/tile_size;
    tile = (r/tile_size)*tile_num + tile;
    // printf("tile %d r %d c %d \n",tile, r, c );
    row_vec[tile].push_back(r);
    col_vec[tile].push_back(c);
    weight_vec[tile].push_back(w);
    count++;
  }

  //############ non concecutive
  // int num_each = 2;
  int num_each = 16;

  vector<float> *row_vec_pad = new vector<float>[tile_num*tile_num];
  vector<float> *col_vec_pad = new vector<float>[tile_num*tile_num];
  vector<float> *weight_vec_pad = new vector<float>[tile_num*tile_num];

  //TILING
  for (int i=0; i<tile_num*tile_num; i++){

    int size = row_vec[i].size();
    // printf("tile%d nonzero = %d\n",i, size );
    if(size==0)
      continue;
    // printf("===========\n");
    int num_group = ((size+num_each-1)/size);
    vector<float> *temp_row = new vector<float>[size];
    vector<float> *temp_col = new vector<float>[size];
    vector<float> *temp_wei = new vector<float>[size];
    int group_count;
    int actual_group_count = 0;
    for (int j=0; j<row_vec[i].size(); j++){
      int inserted = 0;
      group_count = 0;
      while(!inserted){
        if(!(find(temp_row[group_count].begin(), temp_row[group_count].end(), row_vec[i][j]) != temp_row[group_count].end()) && temp_row[group_count].size()<num_each){
          temp_row[group_count].push_back(row_vec[i][j]);
          temp_col[group_count].push_back(col_vec[i][j]);
          temp_wei[group_count].push_back(weight_vec[i][j]);
          // printf("insert %d to group %d\n", row_vec[i][j], group_count);
          inserted = 1;
        }
        group_count++;
        if (actual_group_count < group_count)
          actual_group_count = group_count;
      }
    }
      //printf("=========\n" );
      int group_count2=0;
      while(group_count2<actual_group_count){
        while(temp_row[group_count2].size()!=0 && temp_row[group_count2].size()<num_each){
          // printf("%lu\n",temp_row[num_group-1].size());
          // printf("group_count %d tile %d\n",group_count2, i );
          temp_row[group_count2].push_back(nr);
          temp_col[group_count2].push_back(0);
          temp_wei[group_count2].push_back(0);
        }
      group_count2++;
    }
      //printf("group count%d\n",actual_group_count );
      for(int k=0; k< actual_group_count; k++){
        for(int x=0; x<num_each; x++){
          row_vec_pad[i].push_back(temp_row[k][x]);
          // printf("temp row %d,%d %d\n",k,x,temp_row[k][x] );
          col_vec_pad[i].push_back(temp_col[k][x]);
          weight_vec_pad[i].push_back(temp_wei[k][x]);
        }
      }
    }

    // tile = [0,1] row = [0,1,1,nr]
    //       [1,1]         ary[0]=[1,1,1,0]
  cout << "TILING FINISHED" << endl;

  int vec_size=0;
  for(int i; i<tile_num*tile_num; i++){
    vec_size += row_vec_pad[i].size();
  }
  cout << "construct arrays" << endl;
  int *Row = new int[vec_size];
  int *Col = new int[vec_size];
  float *Val = new float[vec_size];
  int mask_size = (vec_size+15)/16;
  __mmask16*  padding_masks = new __mmask16[mask_size];

  int idx=0;
  int num_sixteen = 0;
  int element=0;
  for(int i=0; i<tile_num*tile_num; i++){
    for(int j=0; j<row_vec_pad[i].size(); j++){
      Row[idx] = row_vec_pad[i][j];
      Col[idx] = col_vec_pad[i][j];
      Val[idx] = weight_vec_pad[i][j];
      if(Row[idx] > nr || Col[idx] > nr-1){
            cout << "====out==== " << Row[idx] << " " << Col[idx] <<endl;
        }
      idx++;
    }
  }

  float* invec = new float[nr+1];
  float* invec2 = new float[nr+1];
  float* invec_serial = new float[nr+1];
  float* invec_serial2 = new float[nr+1];
  for (int i=0; i<nr+1; i++){
    invec2[i] = 1000000;
    invec[i] = 1000000;
    invec_serial[i] = 1000000;
    invec_serial2[i] = 1000000;
  }
  invec[0] = 0;
  invec2[0] = 0;
  invec_serial[0] = 0;
  invec_serial2[0] = 0;
  assert (count == nnz);

  cout << "===========BF_SIMD==============" << endl;
for (int k=0; k<nr; k++){
  cout << "================================ iter" << endl;
  int flag=1;
  for(int i=0; i<vec_size; i+=16){
    __m512 vval = _mm512_load_ps(&Val[i]);
    __m512 zeros = _mm512_set1_ps(0);
    __m512i vcol = _mm512_load_epi32(&Col[i]);
    __m512 vinvec = _mm512_i32gather_ps(vcol, invec, 4);

    __m512 vnewweight = _mm512_add_ps(vinvec, vval);
    __m512i vr = _mm512_load_epi32(&Row[i]);
    // __mmask16 mm = _mm512_cmpeq_ps_mask(vval, one);
    __mmask16 mm = _mm512_cmp_ps_mask(vval, zeros, _MM_CMPINT_GT);

    __m512 vinvec2 = _mm512_mask_i32gather_ps(zeros, mm, vr,invec2, 4);
    // _mm512_mask_i32gather_ps (__m512 src, __mmask16 k, __m512i vindex, void const* base_addr, int scale)
    __mmask16 mcomp = _mm512_mask_cmp_ps_mask(mm, vnewweight, vinvec2, _MM_CMPINT_LT);

    _mm512_mask_i32scatter_ps(invec2, mcomp, vr, vnewweight, 4);
  }
  for(int i=0; i<nr+1; i++){
      if(invec[i] != invec2[i])
	        flag=0;
      invec[i] = invec2[i];
    }
    if(flag)
      break;

}


  cout << "===========BF_SEQUENCIAL==============" << endl;

  for (int k=0; k<nr; k++){
    int flag=1;
    cout << "================================ iter" << endl;
    for (int i=0; i< vec_size; i++) {
        int x = Row[i];
        int v = Col[i];
        int weight = Val[i];
        // printf("%d, %d, %d\n",x,v,weight );
        if (invec_serial[v] + weight < invec_serial2[x]){
            invec_serial2[x] = invec_serial[v] + weight;
        }
    }
    for(int i=0; i<nr+1; i++){
      if(invec_serial[i] != invec_serial2[i])
          flag=0;
      invec_serial[i] = invec_serial2[i];
    }
    if(flag)
      break;
  }


  for(int i=0; i<nr; i++){
    if(invec2[i] != invec_serial2[i]){
      cout << "not match ";
      cout << "invec2[" <<i<<"]=" <<invec2[i] << " ";
      cout << "invec_serial2["<<i<<"]=" << invec_serial2[i] << endl;
      break;
    }
  }

  cout << "invec2[100] " << invec2[100] << " invec2[17] " << invec2[17] << endl;
  cout << "invec_serial2[100] " << invec_serial2[100] << " invec_serial2[17] " << invec_serial2[17] << endl;
  // cout << "loop execution time: " << 1e6*(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) << " usec" << endl;

  return 0;

}
