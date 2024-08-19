#ifndef __OPENRM_STRUCTURE_CAMERA_HPP__
#define __OPENRM_STRUCTURE_CAMERA_HPP__
#include <structure/swapbuffer.hpp>
#include <structure/stamp.hpp>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <cstdint>


namespace rm {

class Camera {

public:
    Camera() {}
    int width;                                              // 相机的读取分辨率宽
    int height;                                             // 相机的读取分辨率高
    int camera_id = 1;                                      // 相机id
    int file_descriptor;                                    // 相机的文件描述符

    bool flip = false;                                      // 是否翻转图像

    SwapBuffer<Frame>* buffer = nullptr;                    // 帧双缓冲区

    uint32_t capture_buffer_num = 0;                        // 图像读取的缓冲区数量
    uint32_t* capture_buffer_size = nullptr;                // 图像读取的缓冲区大小, 用于释放内存
    uint8_t** capture_buffer = nullptr;                     // 图像读取的缓冲区指针

    uint8_t* image_buffer = nullptr;                        // 图像读取的共享内存指针

    uint8_t* rgb_host_buffer = nullptr;                     // 用于推理的主机锁页内存
    uint8_t* rgb_device_buffer = nullptr;                   // 用于推理的设备显存

    std::vector<double> camera_offset;                      // 相机坐标系到云台坐标系的变换

    cv::Mat intrinsic_matrix;                               // 相机内参矩阵
    cv::Mat distortion_coeffs;                              // 相机畸变系数

    Eigen::Matrix<double, 4, 4> Trans_pnp2head;             // 相机到云台的变换矩阵
    Eigen::Matrix<double, 3, 3> Rotate_pnp2head;            // 相机到云台的旋转矩阵
    ~Camera() {
        delete[] capture_buffer;
        delete[] capture_buffer_size;
        delete buffer;
    }

};

}

#endif