#ifndef __OPENRM_UTILS_TF_H__
#define __OPENRM_UTILS_TF_H__

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

// 本tf库中的pnp代表opencv的solvepnp:
//      其方向为x轴向右，y轴向下，z轴向前
//
// 本tf库中的cam代表相机坐标系:
//      其方向为x轴向前，y轴向左，z轴向上
//
// 本tf库中的barrel代表枪管坐标系:
//      其方向为x轴向前，y轴向左，z轴向上
//      其中心在枪管中心，其属性与cam相同
//
// 本tf库中的head代表云台pitch轴的坐标系:
//      云台坐标系考虑了相机或枪管安装的旋转和位置偏移，将其修正统一
//      云台坐标系的中心在pitch轴上，是过yaw轴且垂直于pitch轴的平面与pitch轴的交点
//      
// 本tf库中的axis代表云台yaw轴的坐标系:
//      axis坐标系中心与yaw轴重合，其与世界系仅相差一个yaw角的旋转
//      其方向为x轴向前，y轴向左，z轴向上
//      head坐标系在axis坐标系中的位置为head偏移量
//      
// 本tf库中的world代表世界坐标系:
//     其初始化方向由电控给定，由陀螺仪初始化方向决定
//     其方向为x轴向前，y轴向左，z轴向上
//
// 本tf库中对于欧拉角的定义为:
//     yaw: 绕z轴旋转，pitch: 绕y轴旋转，roll: 绕x轴旋转
// 旋转顺序：
//     装甲板及车辆：yaw -> pitch -> roll
//     相机安装偏移: yaw -> pitch -> roll 

namespace rm {

void tf_Mat3f(const cv::Mat& input_mat, Eigen::Matrix<double, 3, 3>& output_eigen);
void tf_Mat3d(const cv::Mat& input_mat, Eigen::Matrix<double, 3, 3>& output_eigen);
void tf_Vec4d(const cv::Mat& input_mat, Eigen::Matrix<double, 4, 1>& output_eigen);
void tf_Vec4f(const cv::Mat& input_mat, Eigen::Matrix<double, 4, 1>& output_eigen);
void tf_GenMat(const std::vector<double> data, int rows, int cols, cv::Mat& output_mat);
void tf_vec3to4(const Eigen::Matrix<double, 3, 1>& input_vec3, Eigen::Matrix<double, 4, 1>& output_vec4);
void tf_pose2trans(const Eigen::Matrix<double, 4, 1>& input_pose, Eigen::Matrix<double, 4, 4>& output_matrix);
void tf_rt2trans(
    const Eigen::Matrix<double, 4, 1>& input_pose,
    const Eigen::Matrix<double, 3, 3>& input_rotate,
    Eigen::Matrix<double, 4, 4>& output_matrix);
void tf_quater2rotate(
    Eigen::Matrix<double, 3, 3>& matrix,
    const double qx, const double qy, const double qz, const double qw
);
void tf_quater2trans(
    Eigen::Matrix<double, 4, 4>& matrix,
    const double qx, const double qy, const double qz, const double qw
);


double tf_rotation2armoryaw(const Eigen::Matrix<double, 3, 3>& rotate);
double tf_rotation2armorpitch(const Eigen::Matrix<double, 3, 3>& rotate);
double tf_rotation2runeroll(const Eigen::Matrix<double, 3, 3>& rotate);
double tf_rotation2caryaw(const Eigen::Matrix<double, 3, 3>& rotate);

void tf_rotate_pnp2cam(
    Eigen::Matrix<double, 3, 3>& rotate_cam
);

void tf_rotate_pnp2cam(
    const Eigen::Matrix<double, 3, 3>& rotate_pnp, 
    Eigen::Matrix<double, 3, 3>& rotate_cam
);




void tf_rotate_pnp2head(
    Eigen::Matrix<double, 3, 3>& rotate_pnp, 
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);

void tf_rotate_pnp2head(
    const Eigen::Matrix<double, 3, 3>& rotate_pnp, 
    Eigen::Matrix<double, 3, 3>& rotate_head,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);




void tf_rotate_cam2head(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);

void tf_rotate_cam2head(
    const Eigen::Matrix<double, 3, 3>& rotate_cam, 
    Eigen::Matrix<double, 3, 3>& rotate_head,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);



void tf_rotate_head2world(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double yaw,
    const double pitch
);

void tf_rotate_head2world(
    const Eigen::Matrix<double, 3, 3>& rotate_head,
    Eigen::Matrix<double, 3, 3>& rotate_world,
    const double yaw,
    const double pitch
);

void tf_rotate_head2world(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double yaw,
    const double pitch,
    const double roll
);

void tf_rotate_head2world(
    const Eigen::Matrix<double, 3, 3>& rotate_head,
    Eigen::Matrix<double, 3, 3>& rotate_world,
    const double yaw,
    const double pitch,
    const double roll
);

void tf_rotate_yaw2matrix(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double armor_yaw
);



void tf_trans_pnp2cam(
    Eigen::Matrix<double, 4, 4>& matrix_trans
);

void tf_trans_pnp2cam(
    const Eigen::Matrix<double, 4, 1>& pose_pnp,
    Eigen::Matrix<double, 4, 1>& pose_cam
);




void tf_trans_cam2head(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);

void tf_trans_cam2head(
    const Eigen::Matrix<double, 4, 1>& pose_cam,
    Eigen::Matrix<double, 4, 1>& pose_head,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);

void tf_trans_pnp2head(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);

void tf_trans_pnp2head(
    const Eigen::Matrix<double, 4, 1>& pose_pnp,
    Eigen::Matrix<double, 4, 1>& pose_head,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
);

void tf_trans_barrel2head(
    Eigen::Matrix<double, 4, 1>& pose_head,
    const double barrel_dx,
    const double barrel_dy,
    const double barrel_dz,
    const double barrel_yaw = 0,
    const double barrel_pitch = 0
);

void tf_trans_barrel2axis(
    Eigen::Matrix<double, 4, 1>& pose_axis,
    const double pitch,
    const double barrel_dx = 0,
    const double barrel_dy = 0,
    const double barrel_dz = 0,
    const double barrel_yaw = 0,
    const double barrel_pitch = 0,
    const double head_dx = 0,
    const double head_dy = 0,
    const double head_dz = 0
);

void tf_trans_barrel2world(
    Eigen::Matrix<double, 4, 1>& pose_axis,
    const double yaw,
    const double pitch,
    const double barrel_dx = 0,
    const double barrel_dy = 0,
    const double barrel_dz = 0,
    const double barrel_yaw = 0,
    const double barrel_pitch = 0,
    const double head_dx = 0,
    const double head_dy = 0,
    const double head_dz = 0
);


void tf_trans_head2world(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double yaw,
    const double pitch
);

void tf_trans_head2world(
    const Eigen::Matrix<double, 4, 1>& pose_head,
    Eigen::Matrix<double, 4, 1>& pose_world,
    const double yaw,
    const double pitch
);

void tf_trans_head2world(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double yaw,
    const double pitch,
    const double roll
);

void tf_trans_head2world(
    const Eigen::Matrix<double, 4, 1>& pose_head,
    Eigen::Matrix<double, 4, 1>& pose_world,
    const double yaw,
    const double pitch,
    const double roll
);

void tf_trans_head2world(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double yaw,
    const double pitch,
    const double head_dx,
    const double head_dy,
    const double head_dz
);

void tf_trans_head2world(
    const Eigen::Matrix<double, 4, 1>& pose_head,
    Eigen::Matrix<double, 4, 1>& pose_world,
    const double yaw,
    const double pitch,
    const double head_dx,
    const double head_dy,
    const double head_dz
);

void tf_trans_single_yaw(
    Eigen::Matrix<double, 4, 4>& matrix,
    const double yaw,
    const double dx,
    const double dy,
    const double dz
);


}


#endif