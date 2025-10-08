#ifndef __OPENRM_UTILS_DELAY_H__
#define __OPENRM_UTILS_DELAY_H__

namespace rm {

double getFlyDelay(
    double& yaw,
    double& pitch, 
    const double speed,
    const double target_x,
    const double target_y,
    const double target_z,
    const double head_dx, 
    const double head_dy, 
    const double head_dz,
    const double barrel_dx, 
    const double barrel_dy, 
    const double barrel_dz,
    const double barrel_yaw,
    const double barrel_pitch);

double getFlyDelay(
    double& yaw,
    double& pitch, 
    const double speed,
    const double target_x,
    const double target_y,
    const double target_z);

double getRotateDelay(
    const double current_yaw,
    const double target_yaw);

}


#endif