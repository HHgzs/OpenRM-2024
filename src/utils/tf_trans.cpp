#include "utils/tf.h"
#include <cmath>
using namespace rm;
using namespace std;

void rm::tf_trans_pnp2cam(
    Eigen::Matrix<double, 4, 4>& matrix_trans
) {
    matrix_trans << 0,      0,      0.001, 0,
                    -0.001, 0,      0,     0, 
                    0,      -0.001, 0,     0, 
                    0,      0,      0,     1;
}

void rm::tf_trans_pnp2cam(
    const Eigen::Matrix<double, 4, 1>& pose_pnp,
    Eigen::Matrix<double, 4, 1>& pose_cam
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << pose_cam(0), pose_cam(1), pose_cam(2), 1;

    Eigen::Matrix<double, 4, 4> matrix_trans;
    tf_trans_pnp2cam(matrix_trans);

    vec = matrix_trans * vec;
    pose_cam << vec(0), vec(1), vec(2), pose_cam(3);

}

void rm::tf_trans_cam2head(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 4, 4> rotate_cam_yaw, rotate_cam_pitch, rotate_cam_roll;
    Eigen::Matrix<double, 4, 4> trans_cam;

    rotate_cam_yaw << cos(cam_yaw), -sin(cam_yaw), 0, 0,
                      sin(cam_yaw),  cos(cam_yaw), 0, 0,
                                 0,             0, 1, 0,
                                 0,             0, 0, 1;
    
    rotate_cam_pitch << cos(cam_pitch), 0, -sin(cam_pitch), 0,
                                     0, 1,               0, 0,
                        sin(cam_pitch), 0,  cos(cam_pitch), 0,
                                     0, 0,               0, 1; 
    
    rotate_cam_roll << 1, 0,              0,             0,
                       0, cos(cam_roll), -sin(cam_roll), 0,
                       0, sin(cam_roll),  cos(cam_roll), 0,
                       0, 0,              0,             1;

    trans_cam << 1, 0, 0, cam_dx,
                 0, 1, 0, cam_dy,
                 0, 0, 1, cam_dz,
                 0, 0, 0, 1;

    matrix_trans = trans_cam * rotate_cam_yaw * rotate_cam_pitch * rotate_cam_roll;
}

void rm::tf_trans_cam2head(
    const Eigen::Matrix<double, 4, 1>& pose_cam,
    Eigen::Matrix<double, 4, 1>& pose_head,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << pose_cam(0), pose_cam(1), pose_cam(2), 1;

    Eigen::Matrix<double, 4, 4> matrix_trans;
    tf_trans_cam2head(matrix_trans, cam_dx, cam_dy, cam_dz, cam_yaw, cam_pitch, cam_roll);

    vec = matrix_trans * vec;
    pose_head << vec(0), vec(1), vec(2), pose_cam(3);
}


void rm::tf_trans_pnp2head(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 4, 4> pnp2cam, cam2head;
    tf_trans_pnp2cam(pnp2cam);
    tf_trans_cam2head(cam2head, cam_dx, cam_dy, cam_dz, cam_yaw, cam_pitch, cam_roll);
    matrix_trans = cam2head * pnp2cam;
}

void rm::tf_trans_pnp2head(
    const Eigen::Matrix<double, 4, 1>& pose_pnp,
    Eigen::Matrix<double, 4, 1>& pose_head,
    const double cam_dx,
    const double cam_dy,
    const double cam_dz,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << pose_head(0), pose_head(1), pose_head(2), 1;

    Eigen::Matrix<double, 4, 4> matrix_trans;
    tf_trans_pnp2head(matrix_trans, cam_dx, cam_dy, cam_dz, cam_yaw, cam_pitch, cam_roll);

    vec = matrix_trans * vec;
    pose_head << vec(0), vec(1), vec(2), pose_head(3);
}

void tf_trans_barrel2head(
    Eigen::Matrix<double, 4, 1>& pose_head,
    const double barrel_dx,
    const double barrel_dy,
    const double barrel_dz,
    const double barrel_yaw = 0,
    const double barrel_pitch = 0
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << barrel_dx, barrel_dy, barrel_dz, 1;

    Eigen::Matrix<double, 4, 4> rotate_yaw, rotate_pitch;
    rotate_yaw << cos(barrel_yaw), -sin(barrel_yaw), 0, 0,
                  sin(barrel_yaw),  cos(barrel_yaw), 0, 0,
                                0,                0, 1, 0,
                                0,                0, 0, 1;
    
    rotate_pitch << cos(barrel_pitch), 0, -sin(barrel_pitch), 0,
                                    0, 1,                  0, 0,
                    sin(barrel_pitch), 0,  cos(barrel_pitch), 0,
                                    0, 0,                  0, 1;
    pose_head = rotate_yaw * rotate_pitch * vec;
}

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
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << barrel_dx, barrel_dy, barrel_dz, 1;

    Eigen::Matrix<double, 4, 4> rotate_barrel_yaw, rotate_barrel_pitch, rotate_pitch;
    Eigen::Matrix<double, 4, 4> trans_head;

    rotate_pitch << cos(pitch), 0, -sin(pitch), 0,
                             0, 1,           0, 0,
                    sin(pitch), 0,  cos(pitch), 0,
                             0, 0,           0, 1;
    
    rotate_barrel_yaw << cos(barrel_yaw), -sin(barrel_yaw), 0, 0,
                         sin(barrel_yaw),  cos(barrel_yaw), 0, 0,
                                       0,                0, 1, 0,
                                       0,                0, 0, 1;

    rotate_barrel_pitch << cos(barrel_pitch), 0, -sin(barrel_pitch), 0,
                                           0, 1,                  0, 0,
                           sin(barrel_pitch), 0,  cos(barrel_pitch), 0,
                                           0, 0,                  0, 1;

    trans_head << 1, 0, 0, head_dx,
                  0, 1, 0, head_dy,
                  0, 0, 1, head_dz,
                  0, 0, 0, 1;
    
    pose_axis = trans_head * rotate_pitch * rotate_barrel_yaw * rotate_barrel_pitch * vec;
}

void tf_trans_barrel2world(
    Eigen::Matrix<double, 4, 1>& pose_world,
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
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << barrel_dx, barrel_dy, barrel_dz, 1;

    Eigen::Matrix<double, 4, 4> rotate_barrel_yaw, rotate_barrel_pitch;
    Eigen::Matrix<double, 4, 4> rotate_yaw, rotate_pitch;
    Eigen::Matrix<double, 4, 4> trans_head;

    rotate_yaw << cos(yaw), -sin(yaw), 0, 0,
                  sin(yaw),  cos(yaw), 0, 0,
                         0,         0, 1, 0,
                         0,         0, 0, 1;

    rotate_pitch << cos(pitch), 0, -sin(pitch), 0,
                             0, 1,           0, 0,
                    sin(pitch), 0,  cos(pitch), 0,
                             0, 0,           0, 1;
    
    rotate_barrel_yaw << cos(barrel_yaw), -sin(barrel_yaw), 0, 0,
                         sin(barrel_yaw),  cos(barrel_yaw), 0, 0,
                                       0,                0, 1, 0,
                                       0,                0, 0, 1;

    rotate_barrel_pitch << cos(barrel_pitch), 0, -sin(barrel_pitch), 0,
                                           0, 1,                  0, 0,
                           sin(barrel_pitch), 0,  cos(barrel_pitch), 0,
                                           0, 0,                  0, 1;

    trans_head << 1, 0, 0, head_dx,
                  0, 1, 0, head_dy,
                  0, 0, 1, head_dz,
                  0, 0, 0, 1;
    
    pose_world = rotate_yaw * trans_head * rotate_pitch * rotate_barrel_yaw * rotate_barrel_pitch * vec;

}


void rm::tf_trans_head2world(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double yaw,
    const double pitch
) {
    Eigen::Matrix<double, 4, 4> rotate_yaw, rotate_pitch;

    rotate_yaw << cos(yaw), -sin(yaw), 0, 0,
                  sin(yaw),  cos(yaw), 0, 0,
                         0,         0, 1, 0,
                         0,         0, 0, 1;
    
    rotate_pitch << cos(pitch), 0, -sin(pitch), 0,
                             0, 1,           0, 0,
                    sin(pitch), 0,  cos(pitch), 0,
                             0, 0,           0, 1;

    matrix_trans = rotate_yaw * rotate_pitch;
}

void rm::tf_trans_head2world(
    const Eigen::Matrix<double, 4, 1>& pose_head,
    Eigen::Matrix<double, 4, 1>& pose_world,
    const double yaw,
    const double pitch
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << pose_head(0), pose_head(1), pose_head(2), 1;

    Eigen::Matrix<double, 4, 4> matrix_trans;
    tf_trans_head2world(matrix_trans, yaw, pitch);

    vec = matrix_trans * vec;
    pose_world << vec(0), vec(1), vec(2), pose_head(3);
}

void rm::tf_trans_head2world(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double yaw,
    const double pitch,
    const double roll
) {
    Eigen::Matrix<double, 4, 4> rotate_yaw, rotate_pitch, rotate_roll;

    rotate_yaw << cos(yaw), -sin(yaw), 0, 0,
                  sin(yaw),  cos(yaw), 0, 0,
                         0,         0, 1, 0,
                         0,         0, 0, 1;
    
    rotate_pitch << cos(pitch), 0, -sin(pitch), 0,
                             0, 1,           0, 0,
                    sin(pitch), 0,  cos(pitch), 0,
                             0, 0,           0, 1;

    rotate_roll << 1, 0,          0,         0,
                   0, cos(roll), -sin(roll), 0,
                   0, sin(roll),  cos(roll), 0,
                   0, 0,          0,         1;

    matrix_trans = rotate_roll * rotate_yaw * rotate_pitch;
}

void rm::tf_trans_head2world(
    const Eigen::Matrix<double, 4, 1>& pose_head,
    Eigen::Matrix<double, 4, 1>& pose_world,
    const double yaw,
    const double pitch,
    const double roll
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << pose_head(0), pose_head(1), pose_head(2), 1;

    Eigen::Matrix<double, 4, 4> matrix_trans;
    tf_trans_head2world(matrix_trans, yaw, pitch, roll);

    vec = matrix_trans * vec;
    pose_world << vec(0), vec(1), vec(2), pose_head(3);
}

void rm::tf_trans_head2world(
    Eigen::Matrix<double, 4, 4>& matrix_trans,
    const double yaw,
    const double pitch,
    const double head_dx,
    const double head_dy,
    const double head_dz
) {
    Eigen::Matrix<double, 4, 4> rotate_yaw, rotate_pitch, trans_head;

    rotate_yaw << cos(yaw), -sin(yaw), 0, 0,
                  sin(yaw),  cos(yaw), 0, 0,
                         0,         0, 1, 0,
                         0,         0, 0, 1;
    
    rotate_pitch << cos(pitch), 0, -sin(pitch), 0,
                             0, 1,           0, 0,
                    sin(pitch), 0,  cos(pitch), 0,
                             0, 0,           0, 1;

    trans_head << 1, 0, 0, head_dx,
                  0, 1, 0, head_dy,
                  0, 0, 1, head_dz,
                  0, 0, 0, 1;

    matrix_trans = rotate_yaw * trans_head * rotate_pitch;
}


void rm::tf_trans_head2world(
    const Eigen::Matrix<double, 4, 1>& pose_head,
    Eigen::Matrix<double, 4, 1>& pose_world,
    const double yaw,
    const double pitch,
    const double head_dx,
    const double head_dy,
    const double head_dz
) {
    Eigen::Matrix<double, 4, 1> vec;
    vec << pose_head(0), pose_head(1), pose_head(2), 1;

    Eigen::Matrix<double, 4, 4> matrix_trans;
    tf_trans_head2world(matrix_trans, yaw, pitch, head_dx, head_dy, head_dz);

    vec = matrix_trans * vec;
    pose_world << vec(0), vec(1), vec(2), pose_head(3);
}

void rm::tf_trans_single_yaw(
    Eigen::Matrix<double, 4, 4>& matrix,
    const double yaw,
    const double dx,
    const double dy,
    const double dz
) {
    matrix << cos(yaw), -sin(yaw), 0, dx,
              sin(yaw),  cos(yaw), 0, dy,
                     0,         0, 1, dz,
                     0,         0, 0, 1;
}