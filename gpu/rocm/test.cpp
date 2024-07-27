/* -----------------------------------------------------------------------------
 * Copyright (c) 2020 Advanced Micro Devices, Inc. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include <hip/hip_runtime.h>
#include <stdio.h>

__global__
void kernel(const int* pA, const int* pB, int* pC) {
    const auto gidx = blockIdx.x * blockDim.x + threadIdx.x;

    pC[gidx] = pA[gidx] + pB[gidx];
}

int main() {
    int a[]{1, 2, 3, 4, 5};
    int b[]{6, 7, 8, 9, 10};
    int c[sizeof(a) / sizeof(a[0])];

    int* pA{nullptr}; hipMalloc((void**)&pA, sizeof(a));
    int* pB{nullptr}; hipMalloc((void**)&pB, sizeof(b));
    int* pC{nullptr}; hipMalloc((void**)&pC, sizeof(c));

    hipMemcpy(pA, a, sizeof(a));
    hipMemcpy(pB, b, sizeof(b));

    hipLaunchKernelGGL(
        kernel,
        dim3(1),
        dim3(sizeof(a) / sizeof(a[0])),
        0,
        nullptr,
        pA,
        pB,
        pC);

    hipMemcpy(c, pC, sizeof(c));

    for (auto i = 0u; i != sizeof(a) / sizeof(a[0]); ++i) {
      if (c[i] != a[i] + b[i]) throw;
	  else{
	  printf("%d ",c[i]);
	  }
    }

    return 0;
}