#include <stdio.h>

__global__ void hello() {
    printf("Hello from GPU);
}

int main() {
    hello<<<1, 1>>>();
    cudaDeviceSynchronize();
}
