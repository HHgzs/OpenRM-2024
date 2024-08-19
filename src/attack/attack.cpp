#include "attack/attack.h"
#include <Eigen/Dense>

bool rm::isValidArmorID(ArmorID armor_id, char valid_byte) {
    if (armor_id == ARMOR_ID_UNKNOWN) {
        return false;
    }
    if (valid_byte & VALID_BYTE_MASK_ALL) {
        return true;
    }
    switch (armor_id) {
        case ARMOR_ID_SENTRY:
            return valid_byte & VALID_BYTE_MASK_SENTRY;
        case ARMOR_ID_HERO:
            return valid_byte & VALID_BYTE_MASK_HERO;
        case ARMOR_ID_ENGINEER:
            return valid_byte & VALID_BYTE_MASK_ENGINEER;
        case ARMOR_ID_INFANTRY_3:
            return valid_byte & VALID_BYTE_MASK_INFANTRY3;
        case ARMOR_ID_INFANTRY_4:
            return valid_byte & VALID_BYTE_MASK_INFANTRY4;
        case ARMOR_ID_INFANTRY_5:
            return valid_byte & VALID_BYTE_MASK_INFANTRY5;
        case ARMOR_ID_TOWER:
            return valid_byte & VALID_BYTE_MASK_TOWER;
        default:
            return false;
    }
}

double rm::getAngleOffsetTargetToReferee(
    const double yaw,
    const double pitch,
    const double target_x,
    const double target_y,
    const double target_z,
    const double referee_x,
    const double referee_y,
    const double referee_z,
    const double referee_yaw,
    const double referee_pitch,
    const double axis_x,
    const double axis_y,
    const double axis_z
) {
    Eigen::Matrix<double, 3, 3> matrix_rotate;
    Eigen::Matrix<double, 4, 4> matrix_trans;
    Eigen::Matrix<double, 3, 1> vec_referee, vec_target;
    Eigen::Matrix<double, 4, 1> pose_referee;

    vec_referee <<  cos(referee_yaw) * cos(referee_pitch), 
                    sin(referee_yaw) * cos(referee_pitch),  
                    sin(referee_pitch);
    matrix_rotate << cos(yaw) * cos(pitch), -sin(yaw), -cos(yaw) * sin(pitch),
                     sin(yaw) * cos(pitch),  cos(yaw), -sin(yaw) * sin(pitch),
                     sin(pitch),             0,         cos(pitch);
    vec_referee = matrix_rotate * vec_referee;

    pose_referee << referee_x, referee_y, referee_z, 1;
    matrix_trans << cos(yaw) * cos(pitch), -sin(yaw), -cos(yaw) * sin(pitch), axis_x,
                    sin(yaw) * cos(pitch),  cos(yaw), -sin(yaw) * sin(pitch), axis_y,
                    sin(pitch),             0,         cos(pitch),            axis_z,
                    0,                      0,         0,                     1;
    pose_referee = matrix_trans * pose_referee;

    vec_target << target_x - pose_referee(0, 0), 
                  target_y - pose_referee(1, 0),
                  target_z - pose_referee(2, 0);

    double dot_product = vec_referee.dot(vec_target);
    double norm_referee = vec_referee.norm();
    double norm_target = vec_target.norm();
    double cos_theta = dot_product / (norm_referee * norm_target);
    double theta = acos(cos_theta);
    
    return theta;
}