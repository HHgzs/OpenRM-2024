#include "utils/tf.h"
#include <cmath>
using namespace rm;
using namespace std;

void rm::tf_Mat3d(const cv::Mat& input_mat, Eigen::Matrix<double, 3, 3>& output_eigen) {
    output_eigen << input_mat.at<double>(0, 0), input_mat.at<double>(0, 1), input_mat.at<double>(0, 2), 
                    input_mat.at<double>(1, 0), input_mat.at<double>(1, 1), input_mat.at<double>(1, 2), 
                    input_mat.at<double>(2, 0), input_mat.at<double>(2, 1), input_mat.at<double>(2, 2);
}

void rm::tf_Mat3f(const cv::Mat& input_mat, Eigen::Matrix<double, 3, 3>& output_eigen) {
    output_eigen << input_mat.at<float>(0, 0), input_mat.at<float>(0, 1), input_mat.at<float>(0, 2), 
                    input_mat.at<float>(1, 0), input_mat.at<float>(1, 1), input_mat.at<float>(1, 2), 
                    input_mat.at<float>(2, 0), input_mat.at<float>(2, 1), input_mat.at<float>(2, 2);
}

void rm::tf_Vec4d(const cv::Mat& input_mat, Eigen::Matrix<double, 4, 1>& output_eigen) {
    output_eigen << input_mat.at<double>(0, 0), input_mat.at<double>(1, 0), input_mat.at<double>(2, 0), 1;
}

void rm::tf_Vec4f(const cv::Mat& input_mat, Eigen::Matrix<double, 4, 1>& output_eigen) {
    output_eigen << input_mat.at<float>(0, 0), input_mat.at<float>(1, 0), input_mat.at<float>(2, 0), 1;
}

void rm::tf_vec3to4(const Eigen::Matrix<double, 3, 1>& input_vec3, Eigen::Matrix<double, 4, 1>& output_vec4) {
    output_vec4 << input_vec3(0), input_vec3(1), input_vec3(2), 1;
}

void rm::tf_pose2trans(const Eigen::Matrix<double, 4, 1>& input_pose, Eigen::Matrix<double, 4, 4>& output_matrix) {
    output_matrix << 1, 0, 0, input_pose(0),
                     0, 1, 0, input_pose(1),
                     0, 0, 1, input_pose(2),
                     0, 0, 0, 1;
}

void rm::tf_rt2trans(
    const Eigen::Matrix<double, 4, 1>& input_pose,
    const Eigen::Matrix<double, 3, 3>& input_rotate,
    Eigen::Matrix<double, 4, 4>& output_matrix) {
    output_matrix << input_rotate(0, 0), input_rotate(0, 1), input_rotate(0, 2), input_pose(0),
                     input_rotate(1, 0), input_rotate(1, 1), input_rotate(1, 2), input_pose(1),
                     input_rotate(2, 0), input_rotate(2, 1), input_rotate(2, 2), input_pose(2),
                     0, 0, 0, 1;
}


void rm::tf_GenMat(const std::vector<double> data, int rows, int cols, cv::Mat& output_mat) {
    std::vector<float> fdata(data.begin(), data.end());
    output_mat = cv::Mat(fdata).reshape(1, rows).clone();
}

void rm::tf_quater2rotate(
    Eigen::Matrix<double, 3, 3>& matrix,
    const double qx, const double qy, const double qz, const double qw
) {
    double m00 = 1 - 2 * qy * qy - 2 * qz * qz;
    double m01 = 2 * qx * qy - 2 * qz * qw;
    double m02 = 2 * qx * qz + 2 * qy * qw;
    double m10 = 2 * qx * qy + 2 * qz * qw;
    double m11 = 1 - 2 * qx * qx - 2 * qz * qz;
    double m12 = 2 * qy * qz - 2 * qx * qw;
    double m20 = 2 * qx * qz - 2 * qy * qw;
    double m21 = 2 * qy * qz + 2 * qx * qw;
    double m22 = 1 - 2 * qx * qx - 2 * qy * qy;
    matrix << m00, m01, m02,
              m10, m11, m12,
              m20, m21, m22;
}

void rm::tf_quater2trans(
    Eigen::Matrix<double, 4, 4>& matrix,
    const double qx, const double qy, const double qz, const double qw
) {
    double m00 = 1 - 2 * qy * qy - 2 * qz * qz;
    double m01 = 2 * qx * qy - 2 * qz * qw;
    double m02 = 2 * qx * qz + 2 * qy * qw;
    double m10 = 2 * qx * qy + 2 * qz * qw;
    double m11 = 1 - 2 * qx * qx - 2 * qz * qz;
    double m12 = 2 * qy * qz - 2 * qx * qw;
    double m20 = 2 * qx * qz - 2 * qy * qw;
    double m21 = 2 * qy * qz + 2 * qx * qw;
    double m22 = 1 - 2 * qx * qx - 2 * qy * qy;
    matrix << m00, m01, m02, 0,
              m10, m11, m12, 0,
              m20, m21, m22, 0,
              0,   0,   0,   1;  
}

