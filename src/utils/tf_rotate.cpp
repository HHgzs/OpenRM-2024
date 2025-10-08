#include "utils/tf.h"
#include <cmath>
using namespace rm;
using namespace std;

void rm::tf_rotate_pnp2cam(Eigen::Matrix<double, 3, 3>& matrix_rotate) {
    matrix_rotate << 0,  0,  1,
                    -1,  0,  0,
                     0, -1,  0;
}

void rm::tf_rotate_pnp2cam(
    const Eigen::Matrix<double, 3, 3>& rotate_pnp, 
    Eigen::Matrix<double, 3, 3>& rotate_cam
) {
    Eigen::Matrix<double, 3, 3> rotate_flip;
    tf_rotate_pnp2cam(rotate_flip);
    rotate_cam = rotate_flip * rotate_pnp;
}



void rm::tf_rotate_pnp2head(
    Eigen::Matrix<double, 3, 3>& matrix_rotate, 
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 3, 3> rotate_cam_yaw, rotate_cam_pitch, rotate_cam_roll, rotate_flip;

    rotate_cam_yaw << cos(cam_yaw), -sin(cam_yaw), 0,
                      sin(cam_yaw),  cos(cam_yaw), 0,
                                 0,             0, 1;

    rotate_cam_pitch << cos(cam_pitch), 0, -sin(cam_pitch),
                                     0, 1,               0,
                        sin(cam_pitch), 0,  cos(cam_pitch);

    rotate_cam_roll << 1, 0,              0,
                       0, cos(cam_roll), -sin(cam_roll),
                       0, sin(cam_roll),  cos(cam_roll);

    rotate_flip << 0,  0,  1,
                  -1,  0,  0,
                   0, -1,  0;

    matrix_rotate =rotate_cam_yaw * rotate_cam_pitch * rotate_cam_roll * rotate_flip;
}

void rm::tf_rotate_pnp2head(
    const Eigen::Matrix<double, 3, 3>& rotate_pnp, 
    Eigen::Matrix<double, 3, 3>& rotate_head,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 3, 3> matrix_rotate;
    tf_rotate_pnp2head(matrix_rotate, cam_yaw, cam_pitch, cam_roll);
    rotate_head = matrix_rotate * rotate_pnp;
}



void rm::tf_rotate_cam2head(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 3, 3> rotate_cam_yaw, rotate_cam_pitch, rotate_cam_roll;

    rotate_cam_yaw << cos(cam_yaw), -sin(cam_yaw), 0,
                      sin(cam_yaw),  cos(cam_yaw), 0,
                                 0,             0, 1;

    rotate_cam_pitch << cos(cam_pitch), 0, -sin(cam_pitch),
                                     0, 1,               0,
                        sin(cam_pitch), 0,  cos(cam_pitch);

    rotate_cam_roll << 1, 0,              0,
                       0, cos(cam_roll), -sin(cam_roll),
                       0, sin(cam_roll),  cos(cam_roll);

    matrix_rotate = rotate_cam_yaw * rotate_cam_pitch * rotate_cam_roll;
}

void rm::tf_rotate_cam2head(
    const Eigen::Matrix<double, 3, 3>& rotate_cam, 
    Eigen::Matrix<double, 3, 3>& rotate_head,
    const double cam_yaw,
    const double cam_pitch,
    const double cam_roll
) {
    Eigen::Matrix<double, 3, 3> matrix_rotate;
    tf_rotate_cam2head(matrix_rotate, cam_yaw, cam_pitch, cam_roll);
    rotate_head = matrix_rotate * rotate_cam;
}




void rm::tf_rotate_head2world(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double yaw,
    const double pitch
) {
    Eigen::Matrix<double, 3, 3> rotate_yaw, rotate_pitch;
    rotate_yaw << cos(yaw), -sin(yaw), 0,
                  sin(yaw),  cos(yaw), 0,
                         0,         0, 1;

    rotate_pitch << cos(pitch), 0, -sin(pitch),
                            0,  1,           0,
                    sin(pitch), 0,  cos(pitch);

    matrix_rotate = rotate_yaw * rotate_pitch;
}


void rm::tf_rotate_head2world(
    const Eigen::Matrix<double, 3, 3>& rotate_head,
    Eigen::Matrix<double, 3, 3>& rotate_world,
    const double yaw,
    const double pitch
) {
    Eigen::Matrix<double, 3, 3> matrix_rotate;
    tf_rotate_head2world(matrix_rotate, yaw, pitch);
    rotate_world = matrix_rotate * rotate_head;
}

void rm::tf_rotate_head2world(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double yaw,
    const double pitch,
    const double roll
) {
    Eigen::Matrix<double, 3, 3> rotate_yaw, rotate_pitch, rotate_roll;
    rotate_yaw << cos(yaw), -sin(yaw), 0,
                  sin(yaw),  cos(yaw), 0,
                         0,         0, 1;

    rotate_pitch << cos(pitch), 0, -sin(pitch),
                            0,  1,           0,
                    sin(pitch), 0,  cos(pitch);
    
    rotate_roll << 1, 0,          0,
                   0, cos(roll), -sin(roll),
                   0, sin(roll),  cos(roll);
    
    matrix_rotate = rotate_roll * rotate_yaw * rotate_pitch;
}

void rm::tf_rotate_head2world(
    const Eigen::Matrix<double, 3, 3>& rotate_head,
    Eigen::Matrix<double, 3, 3>& rotate_world,
    const double yaw,
    const double pitch,
    const double roll
) {
    Eigen::Matrix<double, 3, 3> matrix_rotate;
    tf_rotate_head2world(matrix_rotate, yaw, pitch, roll);
    rotate_world = matrix_rotate * rotate_head;
}

void rm::tf_rotate_yaw2matrix(
    Eigen::Matrix<double, 3, 3>& matrix_rotate,
    const double armor_yaw
) {
    double armor_pitch = -M_PI / 12.0;
    Eigen::Matrix<double, 3, 3> matrix_armor_pitch, matrix_armor_yaw;

    matrix_armor_pitch << cos(armor_pitch), 0, -sin(armor_pitch),
                                         0, 1,                 0,
                          sin(armor_pitch), 0,  cos(armor_pitch);

    matrix_armor_yaw <<   cos(armor_yaw), -sin(armor_yaw), 0,
                          sin(armor_yaw),  cos(armor_yaw), 0,
                                       0,               0, 1;

    matrix_rotate = matrix_armor_pitch * matrix_armor_yaw;
    
}



double rm::tf_rotation2armoryaw(const Eigen::Matrix<double, 3, 3>& rotate) {
    Eigen::Matrix<double, 3, 1> vec(0, 0, 1);
    vec = rotate * vec;
    return atan2(vec(1), vec(0));
}

double rm::tf_rotation2armorpitch(const Eigen::Matrix<double, 3, 3>& rotate) {
    Eigen::Matrix<double, 3, 1> vec(0, 0, 1);
    vec = rotate * vec;
    return atan2(-vec(2), sqrt(vec(0) * vec(0) + vec(1) * vec(1)));
}

double rm::tf_rotation2runeroll(const Eigen::Matrix<double, 3, 3>& rotate) {
    Eigen::Matrix<double, 3, 1> vec(1, 0, 0);
    vec = rotate * vec;
    return -atan2(vec(1), vec(0));
}

double rm::tf_rotation2caryaw(const Eigen::Matrix<double, 3, 3>& rotate) {
    Eigen::Matrix<double, 3, 1> vec(1, 0, 0);
    vec = rotate * vec;
    return atan2(vec(1), vec(0));
}