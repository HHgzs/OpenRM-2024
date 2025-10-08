#ifndef __OPENRM_STRUCTURE_STAMP_H__
#define __OPENRM_STRUCTURE_STAMP_H__

#include <memory>
#include <vector>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

#include <structure/enums.hpp>
#include <utils/timer.h>

namespace rm {

struct YoloRect {
    std::vector<cv::Point2f>  four_points;   // 四点框
    cv::Rect                  box;           // 矩形框
    float                     confidence;    // 置信度
    int                       class_id = 0;  // 类别id
    int                       color_id = 0;  // 颜色id
};

struct Lightbar {
    std::vector<cv::Point>    contour;       // 灯条轮廓
	cv::RotatedRect           rect;          // 灯条旋转矩形
	double                    length;        // 灯条长度
	double                    angle;         // 灯条角度
    Lightbar() = default;
};

struct Armor {
    ArmorID                   id;            // 装甲板id
    ArmorColor                color;         // 装甲板颜色
    ArmorSize                 size;          // 装甲板大小
    ArmorElevation            elevation;     // 装甲板仰角
    cv::Point2f               center;        // 装甲板中心
	cv::Rect                  rect;          // 装甲板矩形框
    std::vector<cv::Point2f>  four_points;   // 装甲板的四个顶点
    Armor() = default;
};

struct LightbarPair {
    Lightbar                  first;         // 灯条0
    Lightbar                  second;        // 灯条1
    LightbarPair() = default;
    LightbarPair(Lightbar lb0, Lightbar lb1) : first(lb0), second(lb1) {}
};

struct PointPair {
    cv::Point2f               point_up;      // 上端点  
    cv::Point2f               point_down;    // 下端点
    PointPair() = default;
    PointPair(cv::Point2f p0, cv::Point2f p1) : point_up(p0), point_down(p1) {}
};

struct Locate {
    uint64_t  receive_config;
    double    orientation_w;
    double    orientation_x;
    double    orientation_y;
    double    orientation_z;
    double    position_x;
    double    position_y;
    double    position_z;
    double    nav_at_aim_yaw;
    double    curr_aim_yaw;
    double    target_x;
    double    target_y;
    double    target_z;
    uint64_t  send_config;
};

struct Target {
    Eigen::Matrix<double, 4, 1> pose_world;
    double                      armor_yaw_world;
    double                      rune_angle;
    ArmorID                     armor_id;
    ArmorColor                  color;
    ArmorSize                   armor_size;
    ArmorElevation              armor_elevation;
    Target() = default;
};

struct Frame {

    std::shared_ptr<cv::Mat>    image;              // 图像
    TimePoint                   time_point;         // 时间戳

    int                         camera_id;          // 相机id
    int                         width;              // 图像宽度
    int                         height;             // 图像高度

    float                       yaw;                // 云台yaw
    float                       pitch;              // 云台pitch
    float                       roll;               // 云台roll
    
    Locate                      locate;             // 位置信息

    std::vector<YoloRect>       yolo_list;          // yolo检测结果
    std::vector<Armor>          armor_list;         // 装甲板视觉目标
    std::vector<Target>         target_list;        // 位置目标
    
    Frame() = default;
};

}

#endif