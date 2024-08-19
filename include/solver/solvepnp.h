#ifndef __OPENRM_SOLVER_SOLVEPNP_H__
#define __OPENRM_SOLVER_SOLVEPNP_H__
#include <structure/enums.hpp>
#include <structure/camera.hpp>
#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <utils/tf.h>

namespace rm {

constexpr double ANGLE_UP_15 = M_PI / 12;
constexpr double ANGLE_DOWN_15 = -M_PI / 12;
constexpr double ANGLE_UP_75 = 5 * M_PI / 12;

constexpr double ANGLE_BOUNDARY_UP = M_PI / 4;
constexpr double ANGLE_BOUNDARY_DOWN = 0;

class YawPnP {
public:
    YawPnP() {}
    YawPnP(ArmorElevation elevation) : elevation(elevation) {}

    void setWorldPoints(const std::vector<cv::Point3f>& object_points);
    void setImagePoints(const std::vector<cv::Point2f>& image_points);

    double operator()(double append_yaw) const;

    ArmorElevation setElevation(double pitch);
    ArmorElevation setElevation(rm::ArmorID armor_id);
    std::vector<Eigen::Vector4d> getMapping(double append_yaw) const;
    std::vector<Eigen::Vector2d> getProject(const std::vector<Eigen::Vector4d>& P_world) const;
    double getCost(const std::vector<Eigen::Vector2d>& P_project, double append_yaw) const;
    double getPixelCost(const std::vector<Eigen::Vector2d>& P_project, double append_yaw) const;
    double getAngleCost(const std::vector<Eigen::Vector2d>& P_project, double append_yaw) const;

    double getCost(double append_yaw) const;
    double getPixelCost(double append_yaw) const;
    double getAngleCost(double append_yaw) const;

    double getYawByPixelCost(double left, double right, double epsilon) const;
    double getYawByAngleCost(double left, double right, double epsilon) const;
    double getYawByMix(double pixel_yaw, double angle_yaw) const;


    double          sys_yaw;
    Eigen::Vector4d pose;
    ArmorElevation  elevation;

    std::vector<Eigen::Vector2d> P_pixel;      // 四点真实像素坐标
    std::vector<Eigen::Vector4d> P_world;      // 四点正对世界坐标

    Eigen::Matrix3d Kc;                        // 相机内参矩阵
    Eigen::Matrix4d T;                         // 图像坐标系在陀螺仪坐标系下的表示
    Eigen::Matrix4d T_inv;                     // 陀螺仪坐标系在图像坐标系下的表示
};


double solveYawPnP(
    const double yaw,
    Camera* camera,
    Eigen::Vector4d& ret_pose,
    const std::vector<cv::Point3f>& object_points,
    const std::vector<cv::Point2f>& image_points,
    const Eigen::Matrix3d& rotate_head2world,
    const Eigen::Matrix4d& trans_head2world,
    rm::ArmorID armor_id = rm::ARMOR_ID_UNKNOWN,
    bool display_flag = false);

void displayYawPnP(YawPnP* yaw_pnp);

}

#endif