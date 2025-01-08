#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <cudnn.h>
#include <iostream>
#include <memory>
#include <vector>

/**
 * CUDA Runtime Support and GPU Memory Management
 * 
 * This file implements CUDA runtime initialization, GPU memory management,
 * and basic CUDA operations for LLM inference acceleration.
 */

class CudaMemoryPool {
private:
    void* pool_ptr;
    size_t pool_size;
    size_t allocated_size;
    
public:
    CudaMemoryPool(size_t size) : pool_size(size), allocated_size(0) {
        cudaMalloc(&pool_ptr, pool_size);
        std::cout << "CUDA Memory Pool initialized: " << pool_size / (1024*1024) << " MB" << std::endl;
    }
    
    ~CudaMemoryPool() {
        cudaFree(pool_ptr);
    }
    
    void* allocate(size_t size) {
        if (allocated_size + size > pool_size) {
            return nullptr;
        }
        void* ptr = static_cast<char*>(pool_ptr) + allocated_size;
        allocated_size += size;
        return ptr;
    }
    
    void reset() {
        allocated_size = 0;
    }
};

class CudaRuntime {
private:
    int device_id;
    cudaDeviceProp device_prop;
    cublasHandle_t cublas_handle;
    cudnnHandle_t cudnn_handle;
    std::unique_ptr<CudaMemoryPool> memory_pool;
    
public:
    CudaRuntime(int device = 0) : device_id(device) {
        // Initialize CUDA device
        cudaSetDevice(device_id);
        cudaGetDeviceProperties(&device_prop, device_id);
        
        std::cout << "CUDA Device: " << device_prop.name << std::endl;
        std::cout << "Compute Capability: " << device_prop.major << "." << device_prop.minor << std::endl;
        std::cout << "Global Memory: " << device_prop.totalGlobalMem / (1024*1024) << " MB" << std::endl;
        
        // Initialize cuBLAS
        cublasCreate(&cublas_handle);
        
        // Initialize cuDNN
        cudnnCreate(&cudnn_handle);
        
        // Initialize memory pool (1GB)
        memory_pool = std::make_unique<CudaMemoryPool>(1024 * 1024 * 1024);
    }
    
    ~CudaRuntime() {
        cublasDestroy(cublas_handle);
        cudnnDestroy(cudnn_handle);
    }
    
    void* allocateMemory(size_t size) {
        return memory_pool->allocate(size);
    }
    
    void resetMemoryPool() {
        memory_pool->reset();
    }
    
    cublasHandle_t getCublasHandle() { return cublas_handle; }
    cudnnHandle_t getCudnnHandle() { return cudnn_handle; }
    
    void synchronize() {
        cudaDeviceSynchronize();
    }
    
    void printMemoryInfo() {
        size_t free_mem, total_mem;
        cudaMemGetInfo(&free_mem, &total_mem);
        std::cout << "GPU Memory - Free: " << free_mem / (1024*1024) 
                  << " MB, Total: " << total_mem / (1024*1024) << " MB" << std::endl;
    }
};

// Global CUDA runtime instance
static std::unique_ptr<CudaRuntime> g_cuda_runtime;

extern "C" {
    int cuda_init(int device_id) {
        try {
            g_cuda_runtime = std::make_unique<CudaRuntime>(device_id);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "CUDA initialization failed: " << e.what() << std::endl;
            return -1;
        }
    }
    
    void* cuda_allocate(size_t size) {
        if (!g_cuda_runtime) return nullptr;
        return g_cuda_runtime->allocateMemory(size);
    }
    
    void cuda_synchronize() {
        if (g_cuda_runtime) {
            g_cuda_runtime->synchronize();
        }
    }
    
    void cuda_print_memory_info() {
        if (g_cuda_runtime) {
            g_cuda_runtime->printMemoryInfo();
        }
    }
    
    void cuda_cleanup() {
        g_cuda_runtime.reset();
    }
}

// Matrix operations using CUDA
namespace cuda_ops {
    
void matrix_multiply_fp32(const float* A, const float* B, float* C, 
                         int M, int N, int K, cublasHandle_t handle) {
    const float alpha = 1.0f, beta = 0.0f;
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N,
                N, M, K,
                &alpha,
                B, N,
                A, K,
                &beta,
                C, N);
}

void matrix_multiply_fp16(const __half* A, const __half* B, __half* C,
                         int M, int N, int K, cublasHandle_t handle) {
    const __half alpha = __float2half(1.0f), beta = __float2half(0.0f);
    cublasHgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N,
                N, M, K,
                &alpha,
                B, N,
                A, K,
                &beta,
                C, N);
}

} // namespace cuda_ops
