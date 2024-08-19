#include "kalman/interface/outpostV2.h"
#include "utils/print.h"
#include "uniterm/uniterm.h"
#include <cmath>
using namespace std;
using namespace rm;

// [ x, y, z, theta, vx, vy, vz, omega ]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5,  6,    7   ]  [ 0, 1, 2,   3  ]

// [ theta, omega ]   [ theta ]
// [   0  ,   1   ]   [   0   ]

OutpostV2::OutpostV2() {
    t_ = getTime();
    setMatrixQ(0.01, 0.01, 0.01, 0.02, 0.05, 0.05, 0.05, 0.04);
    setMatrixR(0.1, 0.1, 0.1, 0.2);
    setFireValue(0, 0.1, 0.1, 0.1);
    omega_ = SlideAvg<double>(500);
}

void OutpostV2::push(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
    double dt = getDoubleOfS(t_, t);
    if(dt > fire_delay_) {
        update_num_ = 0;
        model_.restart();
        omega_.clear();
    }
    update_num_++;
    t_ = t;
    
    toggle_ = getToggle(pose[3], model_.estimate_X[3]);
    if (isAngleTrans(pose[3], omega_model_.estimate_X[3])) {
        model_.estimate_X[3] = pose[3];
        omega_model_.estimate_X[0] = pose[3];
        return;
    }

    model_.estimate_X[3] = getAngleTrans(pose[3], model_.estimate_X[3]);
    omega_model_.estimate_X[0] = getAngleTrans(pose[3], omega_model_.estimate_X[0]);

    Eigen::Matrix<double, 1, 1> pose_theta(pose[3]);
    omega_funcA_.dt = dt;
    omega_model_.predict(omega_funcA_);
    omega_model_.update(omega_funcH_, pose_theta);

    omega_.push(omega_model_.estimate_X[1]);

    funcA_.dt = dt;
    model_.predict(funcA_);
    model_.update(funcH_, pose);
    model_.estimate_X[7] = (omega_.getAvg() > 0) ? OUTPOST_OMEGA_V2 : -OUTPOST_OMEGA_V2;

    rm::message("antitop count", (int)update_num_);
    rm::message("antitop toggle", toggle_);
}

Eigen::Matrix<double, 4, 1> OutpostV2::getPose(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    double dt = sys_delay + append_delay;

    if (sys_delay > fire_delay_) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }

    double x_center = model_.estimate_X[0] + model_.estimate_X[4] * dt;
    double y_center = model_.estimate_X[1] + model_.estimate_X[5] * dt;
    double z_center = model_.estimate_X[2];
    
    double omega = (omega_.getAvg() > 0) ? OUTPOST_OMEGA_V2 : -OUTPOST_OMEGA_V2;
    double predict_theta = model_.estimate_X[3] + omega * dt;

    double theta = getAngleMin(predict_theta, x_center, y_center);
    double x = x_center - OUTPOST_R_V2 * cos(theta);
    double y = y_center - OUTPOST_R_V2 * sin(theta);

    Eigen::Matrix<double, 4, 1> pose(x, y, z_center, theta);
    return pose;
}

Eigen::Matrix<double, 4, 1> OutpostV2::getCenter(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    double dt = sys_delay + append_delay;

    if (sys_delay > fire_delay_) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    
    double x_center = model_.estimate_X[0] + model_.estimate_X[4] * dt;
    double y_center = model_.estimate_X[1] + model_.estimate_X[5] * dt;
    double z_center = model_.estimate_X[2];

    double omega = (omega_.getAvg() > 0) ? OUTPOST_OMEGA_V2 : -OUTPOST_OMEGA_V2;
    double predict_theta = model_.estimate_X[3] + omega * dt;

    double theta = getAngleMin(predict_theta, x_center, y_center);

    double target_yaw = atan2(y_center, x_center);
    double x = x_center - OUTPOST_R_V2 * cos(target_yaw);
    double y = y_center - OUTPOST_R_V2 * sin(target_yaw);

    Eigen::Matrix<double, 4, 1> pose(x, y, z_center, theta);
    return pose;
}

double OutpostV2::getSafeSub(const double angle1, const double angle2) {
    double angle = angle1 - angle2;
    while(angle > M_PI) angle -= 2 * M_PI;
    while(angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

double OutpostV2::getAngleTrans(const double target_angle, const double src_angle) {
    double dst_angle = src_angle;
    while(getSafeSub(dst_angle, target_angle) > (M_PI / 3)) dst_angle -= (2 * M_PI) / 3;
    while(getSafeSub(target_angle, dst_angle) > (M_PI / 3)) dst_angle += (2 * M_PI) / 3;

    if (dst_angle * target_angle >= 0) return dst_angle;
    if      (dst_angle > (M_PI / 2))  dst_angle -= 2 * M_PI;
    else if (dst_angle < (-M_PI / 2)) dst_angle += 2 * M_PI;
    return dst_angle;
}

double OutpostV2::getAngleMin(double armor_angle, const double x, const double y) {
    double center_angle = atan2(y, x);
    return getAngleTrans(center_angle, armor_angle);
}

int OutpostV2::getToggle(const double target_angle, const double src_angle) {
    double differ_angle = fabs(target_angle - src_angle);
    int differ_toggle = static_cast<int>(round(differ_angle / (2 * M_PI / 3)));
    toggle_ = (toggle_ + differ_toggle) % 3;

    return toggle_;
}

void OutpostV2::getStateStr(std::vector<std::string>& str) {
    str.push_back("OutpostV2");
    str.push_back("  toggle: " + to_string(toggle_));
    str.push_back("  update num: " + to_string(update_num_));
    str.push_back("  omega: " + to_string(model_.estimate_X[7]));
    str.push_back(" ");
}

bool OutpostV2::getFireArmor(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle = getSafeSub(atan2(pose[1], pose[0]), pose[3]);
    double omega = model_.estimate_X[7];

    if (fabs(omega) < (OUTPOST_OMEGA_V2 * 0.5)) return false;
    if ((fabs(angle) < fire_angle_armor_) && (update_num_ > fire_update_)) return true;
    return false;
}

bool OutpostV2::getFireCenter(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle = getSafeSub(atan2(pose[1], pose[0]), pose[3]);
    double omega = model_.estimate_X[7];

    if (fabs(omega) < (OUTPOST_OMEGA_V2 * 0.5)) return false;
    if ((fabs(angle) < fire_angle_center_) && (update_num_ > fire_update_)) return true;
    return false;
}

bool OutpostV2::isAngleTrans(const double target_angle, const double src_angle) {
    double differ_angle = fabs(getSafeSub(target_angle, src_angle));
    if (differ_angle > (M_PI / 3)) return true;
    else return false;
}

void OutpostV2::setMatrixQ(double q0, double q1, double q2, double q3, double q4, double q5, double q6, double q7) {
    model_.Q << q0, 0, 0, 0, 0, 0, 0, 0,
                0, q1, 0, 0, 0, 0, 0, 0,
                0, 0, q2, 0, 0, 0, 0, 0,
                0, 0, 0, q3, 0, 0, 0, 0,
                0, 0, 0, 0, q4, 0, 0, 0,
                0, 0, 0, 0, 0, q5, 0, 0,
                0, 0, 0, 0, 0, 0, q6, 0,
                0, 0, 0, 0, 0, 0, 0, q7;
}

void OutpostV2::setMatrixR(double r0, double r1, double r2, double r3) {
    model_.R << r0, 0, 0, 0,
                0, r1, 0, 0,
                0, 0, r2, 0,
                0, 0, 0, r3;
}

void OutpostV2::setMatrixOmegaQ(double q0, double q1) {
    omega_model_.Q << q0, 0,
                      0, q1;
}

void OutpostV2::setMatrixOmegaR(double r0) {
    omega_model_.R << r0;
}


