#include "kalman/interface/antitopV1.h"
#include "utils/print.h"
#include <cmath>
using namespace std;
using namespace rm;

// [x, y, z, theta, vx, vy, vz, omega, r]    [x, y, z, theta]
// [0, 1, 2,   3,   4,  5,  6,    7,   8]    [0, 1, 2,   3  ]

AntitopV1::AntitopV1() : model_() {
    t_ = getTime();
    setMatrixQ(0.01, 0.01, 0.01, 0.02, 0.05, 0.05, 0.0001, 0.04, 0.001);
    setMatrixR(0.1, 0.1, 0.1, 0.2);
}

AntitopV1::AntitopV1(double r_min, double r_max, int armor_num) : r_min_(r_min), r_max_(r_max), armor_num_(armor_num), model_() {
    t_ = getTime();
    setMatrixQ(0.01, 0.01, 0.01, 0.02, 0.05, 0.05, 0.0001, 0.04, 0.001);
    setMatrixR(0.1, 0.1, 0.1, 0.2);
}

void AntitopV1::push(const Eigen::Matrix<double, 4, 1>& pose, const TimePoint& t, int toggle) {
    double dt = getDoubleOfS(t_, t);
    t_ = t;
    toggle_ = toggle;

    double predict_theta = model_.estimate_X[3] + model_.estimate_X[7] * dt;

    model_.estimate_X[3] = getAngleTrans(pose[3], model_.estimate_X[3], predict_theta);
    model_.estimate_X[2] = z_[toggle];
    model_.estimate_X[8] = r_[toggle];

    funcA_.dt = dt;
    model_.predict(funcA_);
    model_.update(funcH_, pose);
    model_.estimate_X[8] = std::min(std::max(model_.estimate_X[8], r_min_), r_max_);

    z_[toggle] = model_.estimate_X[2];
    r_[toggle] = model_.estimate_X[8];

    double v = sqrt(model_.estimate_X[4] * model_.estimate_X[4] + model_.estimate_X[5] * model_.estimate_X[5]);
    double w = model_.estimate_X[7];

    slidestd_v_.push(v);
    slidestd_w_.push(w);
    if(dt > 0.5) update_num_ = 0;
    update_num_++;
}

Eigen::Matrix<double, 4, 1> AntitopV1::getPose(double delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    if (sys_delay > 0.5) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    double dt = sys_delay + delay;

    double x_center = model_.estimate_X[0] + model_.estimate_X[4] * dt;
    double y_center = model_.estimate_X[1] + model_.estimate_X[5] * dt;
    double predict_theta = model_.estimate_X[3] + model_.estimate_X[7] * dt;
    
    double theta = getAngleMin(predict_theta, x_center, y_center);
    double r = r_[getToggle(theta, predict_theta)];
    double z = z_[getToggle(theta, predict_theta)];
    double x = x_center - r * cos(theta);
    double y = y_center - r * sin(theta);

    Eigen::Matrix<double, 4, 1> pose;
    pose << x, y, z, theta;
    return pose;
}

void AntitopV1::setMatrixQ(double q0, double q1, double q2, double q3, double q4, double q5, double q6, double q7, double q8) {
    model_.Q << q0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, q1, 0, 0, 0, 0, 0, 0, 0,
                0, 0, q2, 0, 0, 0, 0, 0, 0,
                0, 0, 0, q3, 0, 0, 0, 0, 0,
                0, 0, 0, 0, q4, 0, 0, 0, 0,
                0, 0, 0, 0, 0, q5, 0, 0, 0,
                0, 0, 0, 0, 0, 0, q6, 0, 0,
                0, 0, 0, 0, 0, 0, 0, q7, 0,
                0, 0, 0, 0, 0, 0, 0, 0, q8;
}
void AntitopV1::setMatrixR(double r0, double r1, double r2, double r3) {
    model_.R << r0, 0, 0, 0,
                0, r1, 0, 0,
                0, 0, r2, 0,
                0, 0, 0, r3;
}
void AntitopV1::setRadiusRange(double r_min, double r_max) { r_min_ = r_min; r_max_ = r_max; }
void AntitopV1::setArmorNum(int armor_num) { armor_num_ = armor_num; }
void AntitopV1::setStdValue(double std_v, double std_w) { fire_std_v_ = std_v; fire_std_w_ = std_w; }
void AntitopV1::setFireValue(double angle, int fire_update) { 
    fire_angle_ = angle; fire_update_ = static_cast<uint64_t>(fire_update); }

double AntitopV1::getAngleTrans(const double target_angle, const double src_angle) {
    double dst_angle = src_angle;
    while(dst_angle - target_angle > (M_PI / armor_num_)) {
        dst_angle -= (2 * M_PI) / armor_num_;
    }
    while(target_angle - dst_angle > (M_PI / armor_num_)) {
        dst_angle += (2 * M_PI) / armor_num_;
    }
    return dst_angle;
}

double AntitopV1::getAngleTrans(const double target_angle, const double src_angle, double refer_angle) {
    double dst_angle = src_angle;
    while(refer_angle - target_angle > (M_PI / armor_num_)) {
        refer_angle -= (2 * M_PI) / armor_num_;
        dst_angle -= (2 * M_PI) / armor_num_;
    }
    while(target_angle - refer_angle > (M_PI / armor_num_)) {
        refer_angle += (2 * M_PI) / armor_num_;
        dst_angle += (2 * M_PI) / armor_num_;
    }
    return dst_angle;
}

double AntitopV1::getAngleMin(double armor_angle, const double x, const double y) {
    double center_angle = atan2(y, x);
    return getAngleTrans(center_angle, armor_angle);
}

int AntitopV1::getToggle(const double target_angle, const double src_angle) {
    if (armor_num_ < 4) return 0;
    double differ_angle = fabs(target_angle - src_angle);
    int differ_toggle = static_cast<int>(round(2 * differ_angle / M_PI)) % 2;
    return (differ_toggle^toggle_);
}

bool AntitopV1::isStdStable() {
    double std_v = slidestd_v_.getStd();
    double std_w = slidestd_w_.getStd();
    
    if ((std_v < fire_std_v_) && (std_w < fire_std_w_)) {
        return true;
    }
    return false;
}

bool AntitopV1::isFireValid(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle = abs(atan2(pose[1], pose[0]) - pose[3]);
    while(angle > M_PI) angle -= 2 * M_PI;
    while(angle < -M_PI) angle += 2 * M_PI;
    if ((abs(angle) < fire_angle_) && (update_num_ > fire_update_)) return true;
    return false;
}

