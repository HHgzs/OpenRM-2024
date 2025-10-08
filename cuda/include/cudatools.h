#ifndef __OPENRM_CUDA_TOOLS_H__
#define __OPENRM_CUDA_TOOLS_H__
#include <cstdint>

namespace rm {

void resize(
    uint8_t* src,
    int src_width,
    int src_height,
    float* dst,
    int dst_width,
    int dst_height,
    void* cuda_stream
);

}

#endif