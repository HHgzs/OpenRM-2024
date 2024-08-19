#include <map>
#include <string>
#include <iostream>
#include "uniterm/uniterm.h"
#include "utils/timer.h"
#include "structure/stamp.hpp"
#include "structure/camera.hpp"
#include "video/video.h"
#include "video/daheng/GxIAPI.h"
#include "video/daheng/DxImageProc.h"

using namespace rm;
using namespace std;

static std::map<int, GX_DEV_HANDLE> camap;
struct CallbackParam {
    Camera* camera;
    int64_t bayer_type;
    float* yaw;
    float* pitch;
    float* roll;
    bool flip = false;
};

void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* capture_frame) {
    TimePoint time_stamp = getTime();
    CallbackParam* callback_param = reinterpret_cast<CallbackParam*>(capture_frame->pUserParam);
    float yaw = 0, pitch = 0, roll = 0;
    if (callback_param->yaw != nullptr && callback_param->pitch != nullptr && callback_param->roll != nullptr) {
        yaw = *(callback_param->yaw);
        pitch = *(callback_param->pitch);
        roll = *(callback_param->roll);
    }

    Camera *camera = callback_param->camera;
    bool flip = callback_param->flip;
    
    shared_ptr<Frame> frame = make_shared<Frame>();

    frame->image = make_shared<cv::Mat>(camera->height, camera->width, CV_8UC3);
    frame->time_point = time_stamp;
    frame->camera_id = camera->camera_id;
    frame->width = camera->width;
    frame->height = camera->height;
    frame->yaw = yaw;
    frame->pitch = pitch;
    frame->roll = roll;
    

    DX_BAYER_CONVERT_TYPE convert_type = RAW2RGB_NEIGHBOUR;
    DX_PIXEL_COLOR_FILTER color_filter = DX_PIXEL_COLOR_FILTER(callback_param->bayer_type);
    DX_RGB_CHANNEL_ORDER channel_order = DX_ORDER_BGR;

    VxInt32 status = DxRaw8toRGB24Ex(
        (void*)(capture_frame->pImgBuf),
        (void*)(frame->image->data),
        camera->width,
        camera->height,
        convert_type,
        color_filter,
        flip,
        channel_order
    );

    if (status != DX_OK) {
        rm::message("Video DaHeng callback RAW to BGR failed", rm::MSG_ERROR);
        return;
    }

    camera->buffer->push(frame);
}

bool rm::getDaHengCameraNum(int& num) {
    // 初始化库
    GX_STATUS status = GXInitLib();
    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng init lib failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }

    uint32_t device_num;
    status = GXUpdateDeviceList(&device_num, 1000);
    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng get devices list failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }
    num = static_cast<int>(device_num); 
    return true;
}

bool rm::setDaHengArgs(
    Camera *camera,
    double exposure,
    double gain,
    double fps
) {
    GX_DEV_HANDLE device = camap[camera->camera_id];
    GX_STATUS status;

    // 设置相机参数
    GX_FLOAT_RANGE exposure_time_range;
    GX_FLOAT_RANGE gain_range;
    GX_FLOAT_RANGE frame_rate_range;
    status = GXGetFloatRange(device, GX_FLOAT_EXPOSURE_TIME, &exposure_time_range);
    status = GXGetFloatRange(device, GX_FLOAT_GAIN, &gain_range);
    status = GXGetFloatRange(device, GX_FLOAT_ACQUISITION_FRAME_RATE, &frame_rate_range);
    exposure = clamp(exposure, exposure_time_range.dMin, exposure_time_range.dMax);
    gain = clamp(gain, gain_range.dMin, gain_range.dMax);
    fps = clamp(fps, frame_rate_range.dMin, frame_rate_range.dMax);
    
    // 曝光
    status = GXSetEnum(device, GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_OFF);
    status = GXSetEnum(device, GX_ENUM_EXPOSURE_MODE, GX_EXPOSURE_MODE_TIMED);
    status = GXSetFloat(device, GX_FLOAT_EXPOSURE_TIME, exposure);
    
    // 增益
    status = GXSetEnum(device, GX_ENUM_GAIN_AUTO, GX_GAIN_AUTO_OFF);
    status = GXSetFloat(device, GX_FLOAT_GAIN, gain);

    // 帧率
    status = GXSetEnum(device, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    status = GXSetEnum(device, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
    status = GXSetEnum(device, GX_ENUM_ACQUISITION_FRAME_RATE_MODE, GX_ACQUISITION_FRAME_RATE_MODE_ON);
    status = GXSetFloat(device, GX_FLOAT_ACQUISITION_FRAME_RATE, fps);

    // 白平衡 黑电平
    status = GXSetEnum(device, GX_ENUM_BLACKLEVEL_AUTO, GX_BLACKLEVEL_AUTO_CONTINUOUS);
    status = GXSetEnum(device, GX_ENUM_BALANCE_WHITE_AUTO, GX_BALANCE_WHITE_AUTO_CONTINUOUS);

    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng set camera parameters failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }
    return true;
}

bool rm::openDaHeng(
    Camera *camera,
    int device_num,
    float *yaw_ptr,
    float *pitch_ptr,
    float *roll_ptr,
    bool flip,
    double exposure,
    double gain,
    double fps
) {
    // 初始化camera对象
    if(camera == nullptr) {
        rm::message("Video DaHeng error at nullptr camera", rm::MSG_ERROR);
        return false;
    }
    if (camera->buffer != nullptr) {
        delete camera->buffer;
    }
    camera->buffer = new SwapBuffer<Frame>();
    camera->camera_id = device_num;
    

    // 打开设备
    GX_OPEN_PARAM open_param;
    open_param.accessMode = GX_ACCESS_EXCLUSIVE;
    open_param.openMode = GX_OPEN_INDEX;

    char* num_char = const_cast<char*>(to_string(device_num).c_str());
    open_param.pszContent = num_char;

    GX_DEV_HANDLE device;
    GX_STATUS status = GXOpenDevice(&open_param, &device);
    
    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng open camera failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }
    
    // 将设备句柄存入map
    camap[device_num] = device;

    // 设置包大小
    bool implement_packet_size = false;
    uint32_t packet_size = 0;
    status = GXIsImplemented(device, GX_INT_GEV_PACKETSIZE, &implement_packet_size);
    if (implement_packet_size) {
        status = GXSetInt(device, GX_INT_GEV_PACKETSIZE, packet_size);
    }
    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng set package size failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }

    // 获取图像尺寸
    int64_t image_width;
    int64_t image_height;
    status = GXGetInt(device, GX_INT_WIDTH, &image_width);
    status = GXGetInt(device, GX_INT_HEIGHT, &image_height);
    camera->width = static_cast<int>(image_width);
    camera->height = static_cast<int>(image_height);

    // 设置相机参数
    rm::setDaHengArgs(camera, exposure, gain, fps);

    // 设置缓冲区数量
    status = GXSetAcqusitionBufferNumber(device, 10);
    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng set buffer number failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }

    // 设置回调函数
    int64_t bayer_type;
    status = GXGetEnum(device, GX_ENUM_PIXEL_COLOR_FILTER, &bayer_type);

    CallbackParam* callback_param = new CallbackParam;
    callback_param->bayer_type = bayer_type;
    callback_param->camera = camera;
    callback_param->yaw = yaw_ptr;
    callback_param->pitch = pitch_ptr;
    callback_param->roll = roll_ptr;
    callback_param->flip = flip;

    status = GXRegisterCaptureCallback(
            device,
            reinterpret_cast<void*>(callback_param),
            OnFrameCallbackFun
        );

    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng set callback function failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }

    // 发送开采命令
    status = GXSendCommand(device, GX_COMMAND_ACQUISITION_START);
    if (status != GX_STATUS_SUCCESS) {
        rm::message("Video DaHeng start acquisition failed", rm::MSG_ERROR);
        GXCloseLib();
        return false;
    }
    rm::message("Video DaHeng opened", rm::MSG_OK);
    
    return true;
}


bool rm::closeDaHeng() {
    GX_STATUS status;
    for(auto it = camap.begin(); it != camap.end(); it++) {
        GX_DEV_HANDLE device = it->second;
        status = GXSendCommand(device, GX_COMMAND_ACQUISITION_STOP);
        status = GXUnregisterCaptureCallback(device);
        status = GXCloseDevice(device);
        if(status != GX_STATUS_SUCCESS) {
            rm::message("Video DaHeng close device failed", rm::MSG_ERROR);
            GXCloseLib();
            return false;
        }
    }
    GXCloseLib();
    rm::message("Video DaHeng closed", rm::MSG_WARNING);
    return true;
}
