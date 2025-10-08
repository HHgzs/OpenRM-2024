#include "video/video.h"
#include "uniterm/uniterm.h"
#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <opencv2/opencv.hpp>
#include <cstdint>
#include <thread>
#include <memory>

static void capture_thread(rm::Camera* camera, rm::Locate* locate_ptr, int fps) {
    int delay = 1000.0 / static_cast<double>(fps);
    TimePoint last_time = getTime();
    while (true) {
        int sleep_time = delay - static_cast<int>(getNumOfMs(last_time, getTime()));
        if (sleep_time > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        last_time = getTime();

        struct v4l2_buffer buffer;
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(camera->file_descriptor, VIDIOC_DQBUF, &buffer) < 0) {
            continue;
        }

        TimePoint time_stamp = getTime();
        rm::Locate locate;
        if (locate_ptr != nullptr) {
            locate = *(locate_ptr);
        }

        std::shared_ptr<rm::Frame> frame = std::make_shared<rm::Frame>();
        cv::Mat image_yuv = cv::Mat(camera->height, camera->width, CV_8UC2, camera->capture_buffer[buffer.index]);
        cv::Mat image_bgr;
        
        if (ioctl(camera->file_descriptor, VIDIOC_QBUF, &buffer) < 0) {
            continue;
        }
        
        try {
            cv::cvtColor(image_yuv, image_bgr, cv::COLOR_YUV2BGR_YUYV);
        } catch (const cv::Exception& e) {
            std::string error_msg = e.what();
            rm::message("Video UVC: cvt error at" + error_msg, rm::MSG_ERROR);
            continue;
        } catch (...) {
            rm::message("Video UVC: cvt error", rm::MSG_ERROR);
            continue;
        }

        frame->time_point = time_stamp;
        frame->camera_id = camera->camera_id;
        frame->width = camera->width;
        frame->height = camera->height;
        frame->locate = locate;
        frame->image = std::make_shared<cv::Mat>(image_bgr);
        
        camera->buffer->push(frame);
    }
}

bool rm::testUVC(std::string& device_name) {
    const char* name = device_name.c_str();
    int fd = open(name, O_RDWR);
    if (fd == -1) {
        rm::message("Warning at opening camera: " + device_name, rm::MSG_WARNING);
        close(fd);
        return false;
    }

    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        rm::message("Warning at querying camera: " + device_name, rm::MSG_WARNING);
        close(fd);
        return false;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        rm::message("The device does not handle video capture: " + device_name, rm::MSG_WARNING);
        close(fd);
        return false;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        rm::message("The device does not handle streaming i/o: " + device_name, rm::MSG_WARNING);
        close(fd);
        return false;
    }

    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtdesc.index = 0;
    if (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) < 0) {
        rm::message("Warning at querying camera format: " + device_name, rm::MSG_WARNING);
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

bool rm::listUVC(std::vector<std::string>& device_list, std::string base_name) {
    device_list.clear();
    const std::string devPath = "/dev/";
    const std::string videoPrefix = base_name;

    DIR* dir = opendir(devPath.c_str());
    if (dir == nullptr) {
        rm::message("Failed to open directory: " + devPath, rm::MSG_ERROR);
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fileName = entry->d_name;
        if (fileName.substr(0, videoPrefix.size()) == videoPrefix) {
            std::string deviceName = devPath + fileName;
            if (rm::testUVC(deviceName)) {
                device_list.push_back(deviceName);
            }
        }
    }

    closedir(dir);
    rm::message("Found " + std::to_string(device_list.size()) + " UVC devices.");
    return true;
}


bool rm::openUVC(Camera *camera, unsigned int width, unsigned int height, unsigned int fps, unsigned int buffer_num, std::string device_name) {
    if (camera == nullptr) {
        rm::message("Video UVC error at nullptr camera", rm::MSG_ERROR);
        return false;
    }
    
    
    // 打开视频设备文件
    const char* chname = device_name.c_str();
    int fd = open(chname, O_RDWR);
    if (fd == -1) {
        rm::message("Video UVC error opening: " + device_name, rm::MSG_ERROR);
        close(fd);
        return false;
    }

    // 设置摄像头的参数
    struct v4l2_format format = {0};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = width;
    format.fmt.pix.height = height;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (ioctl(fd, VIDIOC_S_FMT, &format) < 0) {
        rm::message("Video UVC error setting format: " + device_name, rm::MSG_ERROR);
        close(fd);
        return false;
    }

    // 设置帧率
    struct v4l2_streamparm streamparam;
    streamparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    unsigned int denominator = (fps > 45u) ? 60u : 30u;
    streamparam.parm.capture.timeperframe.numerator = 1;
    streamparam.parm.capture.timeperframe.denominator = denominator;
    if (ioctl(fd, VIDIOC_S_PARM, &streamparam) < 0) {
        rm::message("Video UVC error getting stream parameters: " + device_name, rm::MSG_ERROR);
        close(fd);
        return false;
    }

    // 申请内核空间
    camera->capture_buffer_num = std::clamp(buffer_num, 4u, 128u);
    struct v4l2_requestbuffers requestbuffers;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.count = camera->capture_buffer_num;
    requestbuffers.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &requestbuffers) < 0) {
        rm::message("Video UVC error requesting buffer: " + device_name, rm::MSG_ERROR);
        close(fd);
        return false;
    }

    // 映射内核空间到用户空间
    camera->capture_buffer = new uint8_t*[camera->capture_buffer_num];
    camera->capture_buffer_size = new uint32_t[camera->capture_buffer_num];
    struct v4l2_buffer mapbuffer;
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for(uint32_t i = 0; i < camera->capture_buffer_num; i++) {
        mapbuffer.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer) < 0) {
            rm::message("Video UVC error querying buffer: " + device_name, rm::MSG_ERROR);
            delete[] camera->capture_buffer;
            delete[] camera->capture_buffer_size;
            close(fd);
            return false;
        }

        camera->capture_buffer[i] = (uint8_t*)mmap(NULL, mapbuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mapbuffer.m.offset);
        camera->capture_buffer_size[i] = mapbuffer.length;

        if (ioctl(fd, VIDIOC_QBUF, &mapbuffer) < 0) {
            rm::message("Video UVC error mmap buffer: " + device_name, rm::MSG_ERROR);
            for (uint32_t j = 0; j < i; j++) {
                munmap(camera->capture_buffer[j], camera->capture_buffer_size[j]);
            }
            delete[] camera->capture_buffer;
            delete[] camera->capture_buffer_size;
            close(fd);
            return false;
        }
    }

    // 设置Camera参数
    camera->file_descriptor = fd;
    camera->width = static_cast<int>(width);
    camera->height = static_cast<int>(height);
    if (camera->buffer != nullptr) {
        delete camera->buffer;
    }
    camera->buffer = new rm::SwapBuffer<rm::Frame>();
    
    int temp_id = 0;
    int start_index = device_name.length() - 1;
    while(start_index >= 0 && std::isdigit(device_name[start_index])) {
        temp_id = temp_id * 10 + (device_name[start_index] - '0');
        --start_index;
    }
    
    camera->camera_id = 0;
    while (temp_id > 0) {
        camera->camera_id = camera->camera_id * 10 + temp_id % 10;
        temp_id /= 10;
    }

    rm::message("Video UVC opened: " + device_name, rm::MSG_OK);

    return true;
}

bool rm::setUVC(
    Camera *camera,
    int exposure,
    int brightness,
    int contrast,
    int gamma,
    int gain,
    int sharpness,
    int backlight
) {
    struct v4l2_control ctrl; 
    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl.value = V4L2_EXPOSURE_MANUAL;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting exposure mode", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl.value = exposure;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting exposure value", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_BRIGHTNESS;
    ctrl.value = brightness;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting brightness", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_CONTRAST;
    ctrl.value = contrast;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting contrast", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_GAMMA;
    ctrl.value = gamma;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting gamma", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_GAIN;
    ctrl.value = gain;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting gain", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_SHARPNESS;
    ctrl.value = sharpness;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting sharpness", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_BACKLIGHT_COMPENSATION;
    ctrl.value = backlight;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting backlight", rm::MSG_ERROR);
        return false;
    }

    ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
    ctrl.value = 1;
    if (ioctl(camera->file_descriptor, VIDIOC_S_CTRL, &ctrl) < 0) {
        rm::message("Video UVC error setting white balance mode", rm::MSG_ERROR);
        return false;
    }

    return true;
}

bool rm::runUVC(Camera *camera, Locate* locate_ptr, int fps) {
    if (camera == nullptr) {
        rm::message("Video UVC error at nullptr camera", rm::MSG_ERROR);
        return false;
    }

    // 开始采集
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(camera->file_descriptor, VIDIOC_STREAMON, &type) < 0) {
        rm::message("Video UVC error starting stream", rm::MSG_ERROR);
        return false;
    }

    // 启动采集线程
    std::thread capture(&capture_thread, camera, locate_ptr, fps);
    capture.detach();
    rm::message("Video UVC start capture: " + std::to_string(camera->camera_id), rm::MSG_OK);
    return true;
}

bool rm::closeUVC(Camera *camera) {
    if (camera == nullptr) {
        rm::message("Video UVC error at nullptr camera", rm::MSG_ERROR);
        return false;
    }

    // 停止采集
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(camera->file_descriptor, VIDIOC_STREAMOFF, &type) < 0) {
        rm::message("Video UVC error stopping stream", rm::MSG_ERROR);
        return false;
    }

    // 释放内核空间
    for (uint32_t i = 0; i < camera->capture_buffer_num; i++) {
        munmap(camera->capture_buffer[i], camera->capture_buffer_size[i]);
    }
    delete[] camera->capture_buffer;
    delete[] camera->capture_buffer_size;
    camera->capture_buffer = nullptr;
    camera->capture_buffer_size = nullptr;

    // 关闭设备
    close(camera->file_descriptor);
    delete camera->buffer;
    camera->buffer = nullptr;

    rm::message("Video UVC closed: " + std::to_string(camera->camera_id), rm::MSG_WARNING);
    return true;
}