#include "resize.cuh"
#include <algorithm>
#include <cmath>

AffineMatrix input_to_infer;
AffineMatrix infer_to_input;

void generate_affine_matrix(int src_width, int src_height, int dst_width, int dst_height) {
    // 获取比例系数
    float scaleIN = std::min(static_cast<float>(dst_height) / src_height, static_cast<float>(dst_width) / src_width);
		float scaleOUT = 1.f / scaleIN;

    // 计算从input到infer仿射变换矩阵各点的值
    input_to_infer.value[0] = scaleIN;
    input_to_infer.value[1] = 0;
    input_to_infer.value[2] = -scaleIN * src_width * 0.5 + dst_width * 0.5;
    input_to_infer.value[3] = 0;
    input_to_infer.value[4] = scaleIN;
    input_to_infer.value[5] = -scaleIN * src_height * 0.5 + dst_height * 0.5;

		// 计算从infer到input仿射变换矩阵各点的值
		infer_to_input.value[0] = scaleOUT;
		infer_to_input.value[1] = 0;
		infer_to_input.value[2] = -scaleOUT * dst_width * 0.5 + src_width * 0.5;
		infer_to_input.value[3] = 0;
		infer_to_input.value[4] = scaleOUT;
		infer_to_input.value[5] = -scaleOUT * dst_height * 0.5 + src_height * 0.5;
}