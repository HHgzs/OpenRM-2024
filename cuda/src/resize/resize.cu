#include "resize.cuh"

void rm::resize(
    uint8_t* src,
    int src_width,
    int src_height,
    float* dst,
    int dst_width,
    int dst_height,
    void* _cuda_stream
) {
    cudaStream_t cuda_stream = (cudaStream_t)_cuda_stream;
    // 生成仿射变换矩阵
    generate_affine_matrix(src_width, src_height, dst_width, dst_height);

    // 计算线程块和线程数量
    int jobs = dst_width * dst_height;
    int threads = 256;
    int blocks = ceil(jobs / (float)threads);
    
    // 启动核函数
    warpaffine_kernel<<<blocks, threads, 0, cuda_stream>>>(
        src,
        src_width * 3,
        src_width,
        src_height,
        dst,
        dst_width,
        dst_height,
        114,
        infer_to_input,
        jobs
    );

    // 等待核函数执行完成
    cudaStreamSynchronize(cuda_stream);
}