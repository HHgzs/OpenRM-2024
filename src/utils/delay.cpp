#include "utils/delay.h"
#include <algorithm>
#include <cmath>
using namespace std;

double rm::getFlyDelay(
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
    const double barrel_pitchs
) {
    return 0.0;
}

double rm::getFlyDelay(
    double& yaw,
    double& pitch, 
    const double speed,
    const double target_x,
    const double target_y,
    const double target_z
) {
    yaw = atan2(target_y, target_x);
    double g = 9.8;
    double h = target_z;
    double d = sqrt(target_x * target_x + target_y * target_y);
    double t = sqrt(d * d + h * h) / speed;

    for(int i = 0; i < 5; i++) {
        
        pitch = asin((h + 0.5 * g * t * t) / (speed * t));
        
        if (std::isnan(pitch)) {
            pitch = 0.0;
        }

        t = d / (speed * cos(pitch));
    }
    return t;
}

double rm::getRotateDelay(
    const double current_yaw,
    const double target_yaw
) {
    return 0.0;
}