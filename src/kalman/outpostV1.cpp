#include "kalman/interface/outpostV1.h"
#include "utils/print.h"
#include "uniterm/uniterm.h"
#include <cmath>
using namespace std;
using namespace rm;

// [ x, y, z, theta， omega]  [ x, y, z, theta]
// [ 0, 1, 2,   3,      4  ]  [ 0, 1, 2,   3  ]

// [ theta, omega ]   [ theta ]
// [   0  ,   1   ]   [   0   ]

OutpostV1::OutpostV1(bool enable_weighted) : enable_weighted_(enable_weighted) {
    t_ = getTime();
    setMatrixQ(0.01, 0.01, 0.01, 0.02, 0.05);
    setMatrixR(0.1, 0.1, 0.1, 0.2);
    setFireValue(0, 0.1, 0.1, 0.1);
    center_x_   = SlideAvg<double>(500);
    center_y_   = SlideAvg<double>(500);
    center_z_   = SlideAvg<double>(500);
    omega_      = SlideAvg<double>(500);
    weighted_z_ = SlideWeightedAvg<double>(500);
}

void OutpostV1::push(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
    double dt = getDoubleOfS(t_, t);
    if(dt > fire_delay_) {
        update_num_ = 0;
        model_.restart();
        center_x_.clear();
        center_y_.clear();
        center_z_.clear();
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
    
    center_x_.push(model_.estimate_X[0]);
    center_y_.push(model_.estimate_X[1]);
    center_z_.push(model_.estimate_X[2]);
    omega_.push(omega_model_.estimate_X[1]);

    funcA_.dt = dt;
    model_.predict(funcA_);
    model_.update(funcH_, pose);
    model_.estimate_X[4] = (omega_.getAvg() > 0) ? OUTPOST_OMEGA : -OUTPOST_OMEGA;

    weighted_z_.push(pose[2], getWeightByTheta(pose[3]));

    rm::message("antitop count", (int)update_num_);
    rm::message("antitop toggle", toggle_);
}

Eigen::Matrix<double, 4, 1> OutpostV1::getPose(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    double dt = sys_delay + append_delay;

    if (sys_delay > fire_delay_) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }

    double x_center = center_x_.getAvg();
    double y_center = center_y_.getAvg();
    double z_center;

    if (enable_weighted_) {
        z_center = weighted_z_.getAvg();
    } else {
        z_center = center_z_.getAvg();
    }
    
    double omega = (omega_.getAvg() > 0) ? OUTPOST_OMEGA : -OUTPOST_OMEGA;
    double predict_theta = model_.estimate_X[3] + omega * dt;

    double theta = getAngleMin(predict_theta, x_center, y_center);
    double x = x_center - OUTPOST_R * cos(theta);
    double y = y_center - OUTPOST_R * sin(theta);

    Eigen::Matrix<double, 4, 1> pose(x, y, z_center, theta);
    return pose;
}

Eigen::Matrix<double, 4, 1> OutpostV1::getCenter(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    double dt = sys_delay + append_delay;

    if (sys_delay > fire_delay_) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    
    double x_center = center_x_.getAvg();
    double y_center = center_y_.getAvg();
    double z_center;
    
    if (enable_weighted_) {
        z_center = weighted_z_.getAvg();
    } else {
        z_center = center_z_.getAvg();
    }

    double omega = (omega_.getAvg() > 0) ? OUTPOST_OMEGA : -OUTPOST_OMEGA;
    double predict_theta = model_.estimate_X[3] + omega * dt;

    double theta = getAngleMin(predict_theta, x_center, y_center);

    double target_yaw = atan2(y_center, x_center);
    double x = x_center - OUTPOST_R * cos(target_yaw);
    double y = y_center - OUTPOST_R * sin(target_yaw);

    Eigen::Matrix<double, 4, 1> pose(x, y, z_center, theta);
    return pose;
}

double OutpostV1::getSafeSub(const double angle1, const double angle2) {
    double angle = angle1 - angle2;
    while(angle > M_PI) angle -= 2 * M_PI;
    while(angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

double OutpostV1::getAngleTrans(const double target_angle, const double src_angle) {
    double dst_angle = src_angle;
    while(getSafeSub(dst_angle, target_angle) > (M_PI / 3)) dst_angle -= (2 * M_PI) / 3;
    while(getSafeSub(target_angle, dst_angle) > (M_PI / 3)) dst_angle += (2 * M_PI) / 3;

    if (dst_angle * target_angle >= 0) return dst_angle;
    if      (dst_angle > (M_PI / 2))  dst_angle -= 2 * M_PI;
    else if (dst_angle < (-M_PI / 2)) dst_angle += 2 * M_PI;
    return dst_angle;
}

double OutpostV1::getAngleMin(double armor_angle, const double x, const double y) {
    double center_angle = atan2(y, x);
    return getAngleTrans(center_angle, armor_angle);
}

int OutpostV1::getToggle(const double target_angle, const double src_angle) {
    double differ_angle = fabs(target_angle - src_angle);
    int differ_toggle = static_cast<int>(round(differ_angle / (2 * M_PI / 3)));
    toggle_ = (toggle_ + differ_toggle) % 3;

    return toggle_;
}

double OutpostV1::getWeightByTheta(const double theta) {
    return exp(-pow(theta, 2) * 400);
}

void OutpostV1::getStateStr(std::vector<std::string>& str) {
    str.push_back("OutpostV1");
    str.push_back("  toggle: " + to_string(toggle_));
    str.push_back("  update num: " + to_string(update_num_));
    str.push_back("  omega: " + to_string(model_.estimate_X[4]));
    str.push_back(" ");
}

bool OutpostV1::getFireArmor(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle = getSafeSub(atan2(pose[1], pose[0]), pose[3]);
    double omega = model_.estimate_X[4];

    if (fabs(omega) < (OUTPOST_OMEGA * 0.5)) return false;
    if ((fabs(angle) < fire_angle_armor_) && (update_num_ > fire_update_)) return true;
    return false;
}

bool OutpostV1::getFireCenter(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle = getSafeSub(atan2(pose[1], pose[0]), pose[3]);
    double omega = model_.estimate_X[4];

    if (fabs(omega) < (OUTPOST_OMEGA * 0.5)) return false;
    if ((fabs(angle) < fire_angle_center_) && (update_num_ > fire_update_)) return true;
    return false;
}

bool OutpostV1::isAngleTrans(const double target_angle, const double src_angle) {
    double differ_angle = fabs(getSafeSub(target_angle, src_angle));
    if (differ_angle > (M_PI / 3)) return true;
    else return false;
}

void OutpostV1::setMatrixQ(double q0, double q1, double q2, double q3, double q4) {
    model_.Q << q0, 0, 0, 0, 0,
                0, q1, 0, 0, 0,
                0, 0, q2, 0, 0,
                0, 0, 0, q3, 0,
                0, 0, 0, 0, q4;
}

void OutpostV1::setMatrixR(double r0, double r1, double r2, double r3) {
    model_.R << r0, 0, 0, 0,
                0, r1, 0, 0,
                0, 0, r2, 0,
                0, 0, 0, r3;
}

void OutpostV1::setMatrixOmegaQ(double q0, double q1) {
    omega_model_.Q << q0, 0,
                      0, q1;
}

void OutpostV1::setMatrixOmegaR(double r0) {
    omega_model_.R << r0;
}


