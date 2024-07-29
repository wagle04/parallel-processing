

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

// function to initialize the matrix with randm value
void initializeMatrix(int* matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i != j) {
                // only assign random values to non-diagonal elements
                matrix[i * size + j] = rand() % 100 + 1; 
            } else {
                // assign 0 to diagonal elements
                matrix[i * size + j] = 0; 
            }
        }
    }
}

// function to print the matrix
void printMatrix(int* matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cout << matrix[i * size + j] << " ";
        }
        std::cout << std::endl;
    }
}

/* function for CUDA kernel that will be executed by each thread in parallel
performs the updates to the matrix using the Floyd-Warshall algorithm for a specific value of k
*/
__global__ void floydWarshall(int *D, int *vbuf, int *hbuf, int n, int k) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i < n && j < n) {
        vbuf[i] = D[i * n + k];
        hbuf[j] = D[k * n + j];

        // ensure all threads finish updatting vbuf and hbuf before moving to the next iteration
        __syncthreads(); 

        // update the matrix D by shortesst path distance between vertices i and j
        D[i * n + j] = min(D[i * n + j], vbuf[i] + hbuf[j]);
    }
}

int main() {
    // matrix size
    // we can change the matrix size to 100, 1000 and son on in the line below
    const int n = 10;

    // initialize necessary variables
    int *h_D, *d_D, *h_vbuf, *d_vbuf, *h_hbuf, *d_hbuf;

    // allocate and initialize matrix D on host
    h_D = (int*)malloc(n * n * sizeof(int));
    initializeMatrix(h_D, n);

    // allocate and initialize vbuf and hbuf on host
    h_vbuf = (int*)malloc(n * sizeof(int));
    h_hbuf = (int*)malloc(n * sizeof(int));

    // print the original matrix
    std::cout << "Original Matrix:" << std::endl;
    printMatrix(h_D, n);

    // allocate matrix D on device
    cudaMalloc((void**)&d_D, n * n * sizeof(int));

    // allocate vbuf and hbuf on device
    cudaMalloc((void**)&d_vbuf, n * sizeof(int));
    cudaMalloc((void**)&d_hbuf, n * sizeof(int));

    // copy matrix D from host to device
    cudaMemcpy(d_D, h_D, n * n * sizeof(int), cudaMemcpyHostToDevice);

    // set up grid and block dimensions
    dim3 blockDim(16, 16); 
    dim3 gridDim((n + blockDim.x - 1) / blockDim.x, (n + blockDim.y - 1) / blockDim.y);


    for (int k = 0; k < n; ++k) {
        // launch kernel for parallel computation
        floydWarshall<<<gridDim, blockDim>>>(d_D, d_vbuf, d_hbuf, n, k);

        // ensure all threads finish before moving to the next iteration
        cudaDeviceSynchronize(); 
    }

    // copy matrix D from device to host
    cudaMemcpy(h_D, d_D, n * n * sizeof(int), cudaMemcpyDeviceToHost);

    // print the result matrix
    std::cout << "\nResult Matrix:" << std::endl;
    printMatrix(h_D, n);

    // free allocated memory
    free(h_D);
    free(h_vbuf);
    free(h_hbuf);
    cudaFree(d_D);
    cudaFree(d_vbuf);
    cudaFree(d_hbuf);

    return 0;
}