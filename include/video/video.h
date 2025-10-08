#ifndef __OPENRM_VIDEO_VIDEO_H__
#define __OPENRM_VIDEO_VIDEO_H__
#include <structure/camera.hpp>
#include <structure/stamp.hpp>
#include <vector>
#include <string>
#include <cstdint>

namespace rm {

bool getDaHengCameraNum(int& device_num);

bool setDaHengArgs(
    Camera *camera,
    double exposure,
    double gain,
    double fps);

bool openDaHeng(
    Camera *camera,
    int device_num = 1,
    float *yaw = nullptr,
    float *pitch = nullptr,
    float *roll = nullptr,
    bool flip = false,
    double exposure = 2000.0,
    double gain = 15.0,
    double fps = 200.0);

bool closeDaHeng();


bool testUVC(std::string& device_name);

bool listUVC(std::vector<std::string>& device_list, std::string base_name = "video");

bool openUVC(
    Camera *camera,
    unsigned int width = 1920,
    unsigned int height = 1080,
    unsigned int fps = 60,
    unsigned int buffer_num = 8, 
    std::string device_name = "/dev/video0"
);

bool setUVC(
    Camera *camera,
    int exposure,
    int brightness,
    int contrast,
    int gamma,
    int gain,
    int sharpness, 
    int backlight);

bool runUVC(Camera *camera, Locate* locate_ptr, int fps);
bool closeUVC(Camera *camera);

}

#endif