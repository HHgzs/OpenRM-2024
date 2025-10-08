#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <opencv2/opencv.hpp>
#include "uniterm/uniterm.h"

static int xioctl(int fd, int request, void *arg) {
    int r;
    do r = ioctl (fd, request, arg);
    while (-1 == r && EINTR == errno);
    return r;
}

// 获取摄像头支持格式 YUYV, MJPG
static void get_camera_description(int fd) {
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int i = 0;
    while(1) {
        fmtdesc.index = i++;
        if (xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == -1) {
            rm::message("Error querying camera", rm::MSG_ERROR);
            return;
        }
        printf("index=%d\n", fmtdesc.index);
        printf("flags=%d\n", fmtdesc.flags);
        printf("discription=%s\n", fmtdesc.description);
        unsigned char *p = (unsigned char *)&fmtdesc.pixelformat;
        printf("pixelformat=%c%c%c%c\n", p[0], p[1], p[2], p[3]);
        printf("reserved=%d\n", fmtdesc.reserved[0]);
    }
}

// 读取摄像头的参数
static void get_camera_format(int fd) {
    struct v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_G_FMT, &format) == -1) {
        rm::message("Error getting camera format", rm::MSG_ERROR);
        return;
    }

    printf("fmt.pix.width: %d\n", format.fmt.pix.width);
    printf("fmt.pix.height: %d\n", format.fmt.pix.height);
    printf("fmt.pix.pixelformat: %c%c%c%c\n", (format.fmt.pix.pixelformat & 0xFF), 
        ((format.fmt.pix.pixelformat >> 8) & 0xFF), 
        ((format.fmt.pix.pixelformat >> 16) & 0xFF), 
        ((format.fmt.pix.pixelformat >> 24) & 0xFF)); 
}



int main() {

    // 打开视频设备文件
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        rm::message("Error opening camera", rm::MSG_ERROR);
        close(fd);
        return 1;
    }

    // 设置摄像头的参数
    struct v4l2_format format = {0};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = 1280;
    format.fmt.pix.height = 720;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (xioctl(fd, VIDIOC_S_FMT, &format) < 0) {
        rm::message("Error setting camera format", rm::MSG_ERROR);
        close(fd);
        return 1;
    }

    // 申请内核空间
    struct v4l2_requestbuffers requestbuffers;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.count = 4;
    requestbuffers.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &requestbuffers) < 0) {
        rm::message("Error requesting buffer", rm::MSG_ERROR);
        close(fd);
        return 1;
    }

    // 映射内核空间到用户空间
    unsigned char *buffers[4];
    unsigned int buffer_size[4];
    struct v4l2_buffer mapbuffer;
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for(int i = 0; i < 4; i++) {
        mapbuffer.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &mapbuffer) < 0) {
            rm::message("Error querying buffer", rm::MSG_ERROR);
            close(fd);
            return 1;
        }
        buffers[i] = (unsigned char *)mmap(NULL, mapbuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mapbuffer.m.offset);
        buffer_size[i] = mapbuffer.length;
        if (xioctl(fd, VIDIOC_QBUF, &mapbuffer) < 0) {
            rm::message("Error querying buffer", rm::MSG_ERROR);
            close(fd);
            return 1;
        }
    }

    // 开始采集
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        rm::message("Error starting stream", rm::MSG_ERROR);
        close(fd);
        return 1;
    }

    while(1) {
        struct v4l2_buffer readbuffer;
        readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (xioctl(fd, VIDIOC_DQBUF, &readbuffer) < 0) {
            rm::message("Error querying buffer", rm::MSG_ERROR);
            close(fd);
            return 1;
        }

        // opencv获取图像
        cv::Mat frame(720, 1280, CV_8UC2, buffers[readbuffer.index]);

        // 通知内核使用完毕
        if (xioctl(fd, VIDIOC_QBUF, &readbuffer) < 0) {
            rm::message("Error querying buffer", rm::MSG_ERROR);
            close(fd);
            return 1;
        }

        cv::cvtColor(frame, frame, cv::COLOR_YUV2BGR_YUYV);
        cv::imshow("frame", frame);
        cv::waitKey(1);
    }

    if (xioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        rm::message("Error stopping stream", rm::MSG_ERROR);
        close(fd);
        return 1;
    }
    for (int i = 0; i < 4; i++) {
        munmap(buffers[i], buffer_size[i]);
    }
    
}