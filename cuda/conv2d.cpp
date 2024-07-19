#include <iostream>
#include <cudnn.h>

#define BATCH_SIZE 32
#define CHANNELS 3
#define HEIGHT 64
#define WIDTH 64
#define FILTER_SIZE 3
#define NUM_FILTERS 32
#define STRIDE 1
#define PADDING 1
#define POOL_SIZE 2

// Function to create a Convolutional layer
cudnnConvolutionDescriptor_t createConvolutionLayer() {
    cudnnConvolutionDescriptor_t convolutionDesc;
    if (CUDNN_STATUS_SUCCESS != cudnnCreateConvolutionDescriptor(&convolutionDesc))
        std::cerr << "no conv descriptor, sorry :(" << std::endl;

    // https://docs.nvidia.com/deeplearning/cudnn/api/index.html#cudnnSetConvolution2dDescriptor
    // Set the convolution parameters
    cudnnSetConvolution2dDescriptor(convolutionDesc,
                                    PADDING,
                                    PADDING,
                                    STRIDE,
                                    STRIDE,
                                    1, 1, // dilation, not used in this example
                                    CUDNN_CROSS_CORRELATION, // or CUDNN_CONVOLUTION
                                    CUDNN_DATA_FLOAT // compute precision
                                    );

    return convolutionDesc;
}

// Function to create a pooling layer
cudnnPoolingDescriptor_t createPoolingLayer() {
    cudnnPoolingDescriptor_t poolingDesc;
    cudnnCreatePoolingDescriptor(&poolingDesc);

    // Set the pooling parameters
    cudnnSetPooling2dDescriptor(poolingDesc,
                                CUDNN_POOLING_MAX, // pooling mode
                                CUDNN_NOT_PROPAGATE_NAN, // nan propagation mode
                                POOL_SIZE, 
                                POOL_SIZE,
                                0, 0, // padding
                                POOL_SIZE, // stride
                                POOL_SIZE
                                );

    return poolingDesc;
}

void allocateMemory(float** data, size_t size) {
    cudaMalloc(data, size);
}

void deallocateMemory(float* data) {
    cudaFree(data);
}

int main() {
    cudnnHandle_t cudnn;
    cudnnCreate(&cudnn);

    cudnnConvolutionDescriptor_t convolutionDesc = createConvolutionLayer();
    cudnnPoolingDescriptor_t poolingDesc = createPoolingLayer();

    // Allocate memory for input data, filters, and output data
    float* inputData;
    float* filters;
    float* outputData;

    size_t inputSize = BATCH_SIZE * CHANNELS * HEIGHT * WIDTH * sizeof(float);
    size_t filterSize = NUM_FILTERS * CHANNELS * FILTER_SIZE * FILTER_SIZE * sizeof(float);
    size_t outputSize = BATCH_SIZE * NUM_FILTERS * HEIGHT * WIDTH * sizeof(float);

    allocateMemory(&inputData, inputSize);
    allocateMemory(&filters, filterSize);
    allocateMemory(&outputData, outputSize);

    // Create and set the tensor descriptors
    cudnnTensorDescriptor_t inputDesc;
    cudnnCreateTensorDescriptor(&inputDesc);
    cudnnSetTensor4dDescriptor(inputDesc,
                               CUDNN_TENSOR_NCHW,
                               CUDNN_DATA_FLOAT,
                               BATCH_SIZE,
                               CHANNELS,
                               HEIGHT,
                               WIDTH);

    cudnnFilterDescriptor_t filterDesc;
    cudnnCreateFilterDescriptor(&filterDesc);
    cudnnSetFilter4dDescriptor(filterDesc,
                               CUDNN_DATA_FLOAT,
                               CUDNN_TENSOR_NCHW,
                               NUM_FILTERS,
                               CHANNELS,
                               FILTER_SIZE,
                               FILTER_SIZE);

    cudnnTensorDescriptor_t outputDesc;
    cudnnCreateTensorDescriptor(&outputDesc);
    cudnnSetTensor4dDescriptor(outputDesc,
                               CUDNN_TENSOR_NCHW,
                               CUDNN_DATA_FLOAT,
                               BATCH_SIZE,
                               NUM_FILTERS,
                               HEIGHT,
                               WIDTH);

    // ... (set the input, filter, and output data)

    cudnnConvolutionFwdAlgoPerf_t convolutionFwdAlgoPerf;
    cudnnGetConvolutionForwardAlgorithm(cudnn,
                                        inputDesc,
                                        filterDesc,
                                        convolutionDesc,
                                        outputDesc,
                                        CUDNN_CONVOLUTION_FWD_PREFER_FASTEST,
                                        0, // returned algo count
                                        &convolutionFwdAlgoPerf);

    // Set the workspace size for the forward convolution
    size_t workspaceSize;
    cudnnGetConvolutionForwardWorkspaceSize(cudnn,
                                            inputDesc,
                                            filterDesc,
                                            convolutionDesc,
                                            outputDesc,
                                            convolutionFwdAlgoPerf,
                                            &workspaceSize);

    float* workspace;
    cudaMalloc(&workspace, workspaceSize);

    

    float alpha = 1.0f;
    float beta = 0.0f;

    // Perform forward convolution
    cudnnConvolutionForward(cudnn,
                            &alpha,
                            inputDesc,
                            inputData,
                            filterDesc,
                            filters,
                            convolutionDesc,
                            convolutionFwdAlgo,
                            workspace,
                            workspaceSize,
                            &beta,
                            outputDesc,
                            outputData);

    // Perform max pooling
    cudnnPoolingForward(cudnn,
                        poolingDesc,
                        &alpha,
                        outputDesc,
                        outputData,
                        &beta,
                        outputDesc,
                        outputData);

    // ... (perform any additional layers or computations)

    // Cleanup
    cudnnDestroyTensorDescriptor(inputDesc);
    cudnnDestroyFilterDescriptor(filterDesc);
    cudnnDestroyTensorDescriptor(outputDesc);

    cudnnDestroyConvolutionDescriptor(convolutionDesc);
    cudnnDestroyPoolingDescriptor(poolingDesc);

    deallocateMemory(inputData);
    deallocateMemory(filters);
    deallocateMemory(outputData);
    cudaFree(workspace);
    cudnnDestroy(cudnn);

    return 0;
}
