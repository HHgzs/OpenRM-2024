#include "kalman/interface/runeV2.h"
#include "utils/print.h"
#include "uniterm/uniterm.h"
#include "structure/slidestd.hpp"
#include <cmath>
using namespace std;
using namespace rm;

// [ x, y, z, theta, angle, spd ]       [ x, y, z, theta, angle ]
// [ 0, 1, 2,   3,     4,    5  ]       [ 0, 1, 2,   3,     4   ]

// [ x, y, z, theta, angle, p, a, w ]   [ x, y, z, theta, angle ]
// [ 0, 1, 2,   3,     4,   5, 6, 7 ]   [ 0, 1, 2,   3,     4   ]

// [ angle, spd ]   [ angle ]
// [   0  ,  1  ]   [   0   ]

RuneV2::RuneV2() {
    t_ = getTime();
    t_trans_ = getTime();
    setSmallMatrixQ(0.01, 0.01, 0.01, 0.01, 1e-3, 1e-3);
    setSmallMatrixR(1, 1, 1, 1, 1);
    setBigMatrixQ(0.01, 0.01, 0.01, 0.01, 0.01, 0.1, 0.1, 0.1);
    setBigMatrixR(1, 1, 1, 1, 1);
    setSpdMatrixQ(1, 10);
    setSpdMatrixR(1);
    center_x_ = SlideAvg<double>(500);
    center_y_ = SlideAvg<double>(500);
    center_z_ = SlideAvg<double>(500);
    theta_ = SlideAvg<double>(1000);
    spd_ = SlideAvg<double>(500);
}

void RuneV2::push(const Eigen::Matrix<double, 5, 1>& pose, TimePoint t) {
    double dt = getDoubleOfS(t_, t);
    if(dt > 2.0) {
        update_num_ = 0;
        center_x_.clear();
        center_y_.clear();
        center_z_.clear();
        theta_.clear();
        spd_.clear();
        big_model_.restart();
        small_model_.restart();
        spd_model_.restart();
    }
    update_num_++;
    t_ = t;
    

    // 符的不同符叶转换
    double angle = big_model_.estimate_X[4];

    if (is_big_rune_) is_rune_trans_ = getRuneTrans(pose[4], big_model_.estimate_X[4]);
    else              is_rune_trans_ = getRuneTrans(pose[4], small_model_.estimate_X[4]);
    if (is_rune_trans_) t_trans_ = t;
    if (is_rune_trans_) {
        spd_model_.estimate_X[0] = pose[4];
        small_model_.estimate_X[4] = pose[4];
        big_model_.estimate_X[4] = pose[4];
        return;
    }

    spd_model_.estimate_X[0] = getAngleTrans(pose[4], spd_model_.estimate_X[0]);
    small_model_.estimate_X[4] = getAngleTrans(pose[4], small_model_.estimate_X[4]);
    big_model_.estimate_X[4] = getAngleTrans(pose[4], big_model_.estimate_X[4]);
    

    // 符的角速度模型，用于方向判定
    Eigen::Matrix<double, 1, 1> pose_angle(pose[4]);
    spd_funcA_.dt = dt;
    spd_model_.predict(spd_funcA_);
    spd_model_.update(spd_funcH_, pose_angle);
    spd_.push(spd_model_.estimate_X[1]);


    // 小符模型
    small_funcA_.dt = dt;
    small_model_.predict(small_funcA_);
    small_model_.update(small_funcH_, pose);
    small_model_.estimate_X[5] = (spd_.getAvg() > 0) ? SMALL_RUNE_SPD : -SMALL_RUNE_SPD;


    // 大符模型
    big_funcA_.dt = dt;
    big_funcA_.sign = (spd_.getAvg() > 0) ? 1.0: -1.0;

    big_model_.estimate_X[6] = clamp(big_model_.estimate_X[6], A_MIN, A_MAX);
    big_model_.estimate_X[7] = clamp(big_model_.estimate_X[7], W_MIN, W_MAX);

    big_model_.predict(big_funcA_);
    big_model_.update(big_funcH_, pose);


    // 滑动窗口更新
    if (is_big_rune_) {
        center_x_.push(big_model_.estimate_X[0]);
        center_y_.push(big_model_.estimate_X[1]);
        center_z_.push(big_model_.estimate_X[2]);
        theta_.push(big_model_.estimate_X[3]);
    } else {
        center_x_.push(small_model_.estimate_X[0]);
        center_y_.push(small_model_.estimate_X[1]);
        center_z_.push(small_model_.estimate_X[2]);
        theta_.push(small_model_.estimate_X[3]);
    }
}

Eigen::Matrix<double, 4, 1> RuneV2::getPose(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    if (sys_delay > 2.0 || update_num_ < 100) return Eigen::Matrix<double, 4, 1>::Zero();

    double dt = sys_delay + append_delay;
    double x_center, y_center, z_center, theta, angle, spd;
    double a = 0, w = 0, p = 0, b = 0;
    double x, y, z;
    double sign;

    x_center = center_x_.getAvg();
    y_center = center_y_.getAvg();
    z_center = center_z_.getAvg();
    theta = theta_.getAvg();
    sign = (spd_.getAvg() > 0) ? 1.0 : -1.0;

    if (sys_delay > turn_to_center_delay_) return Eigen::Matrix<double, 4, 1>(x_center, y_center, z_center, 0);

    if (is_big_rune_) {
        angle = big_model_.estimate_X[4];

        p = big_model_.estimate_X[5];
        a = big_model_.estimate_X[6];
        w = big_model_.estimate_X[7];
        
        a = clamp(a, A_MIN, A_MAX);
        b = B_BASE - a;
        w = clamp(w, W_MIN, W_MAX);

        
        spd = a * sin(p + w * dt) + b;
        angle += sign * b * dt + sign * a / w * (cos(p) - cos(p + w * dt));

        x = x_center + R * cos(angle) * sin(theta);
        y = y_center - R * cos(angle) * cos(theta);
        z = z_center + R * sin(angle);
        rm::message("rune mode", 'B');
        
    } else {
        spd   = sign * SMALL_RUNE_SPD;
        angle = small_model_.estimate_X[4] + spd * dt;
        x     = x_center + R * cos(angle) * sin(theta);
        y     = y_center - R * cos(angle) * cos(theta);
        z     = z_center + R * sin(angle);
        rm::message("rune mode", 'S');
    }
    
    double center_dist = sqrt(pow(x_center, 2) + pow(y_center, 2) + pow(z_center, 2));

    rm::message("rune center", center_dist);
    rm::message("rune cx", x_center);
    rm::message("rune cy", y_center);
    rm::message("rune cz", z_center);
    rm::message("rune spd", spd);
    rm::message("rune theta", theta * 180 / M_PI);
    rm::message("rune angle", angle * 180 / M_PI);

    if (is_big_rune_) {
        rm::message("rune w", w);
        rm::message("rune a", a);
        rm::message("rune p", p);
    }

    Eigen::Matrix<double, 4, 1> pose(x, y, z, angle);
    // Eigen::Matrix<double, 4, 1> pose(x_center, y_center, z_center, angle);
    return pose;
}

void RuneV2::getStateStr(std::vector<std::string>& str) {
    str.push_back("RuneV2");
    str.push_back(" ");
}

double RuneV2::getSafeSub(const double angle1, const double angle2) {
    double angle = angle1 - angle2;
    while(angle > M_PI) angle -= 2 * M_PI;
    while(angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

bool RuneV2::getRuneTrans(const double angle1, const double angle2) {
    return (fabs(getSafeSub(angle1, angle2)) > (M_PI / 5));
}

double RuneV2::getAngleTrans(const double target_angle, const double src_angle) {
    double dst_angle = src_angle;

    while(getSafeSub(dst_angle, target_angle) > (M_PI / 5)) dst_angle -= (2 * M_PI) / 5;
    while(getSafeSub(target_angle, dst_angle) > (M_PI / 5)) dst_angle += (2 * M_PI) / 5;
    
    while(dst_angle > M_PI)  dst_angle -= 2 * M_PI;
    while(dst_angle < -M_PI) dst_angle += 2 * M_PI;

    if(dst_angle * target_angle >= 0) return dst_angle;
    
    if      (dst_angle > (M_PI / 2))  dst_angle -= 2 * M_PI;
    else if (dst_angle < (-M_PI / 2)) dst_angle += 2 * M_PI;

    return dst_angle;
}


bool RuneV2::getFireFlag(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);
    double trans_delay = getDoubleOfS(t_trans_, now);
    double fire_delay  = getDoubleOfS(t_fire_, now);

    if (is_big_rune_) {
        is_fire_flag_ = false;
        return is_fire_flag_;
    }

    // 刚发生切换，无需发射
    if (trans_delay < fire_after_trans_delay_) {
        is_fire_flag_ = false;
        return is_fire_flag_;
    }

    // 发生切换后，打出第一个上升沿
    if (trans_delay >= fire_after_trans_delay_ && is_rune_trans_) {
        t_fire_ = now;
        is_rune_trans_ = false;
        is_fire_flag_ = true;
        return is_fire_flag_;
    }

    // 超过发射间隔，打出下一个上升沿
    if (trans_delay >= fire_after_trans_delay_ && fire_delay > fire_interval_delay_) {
        t_fire_ = now;
        is_fire_flag_ = true;
        return is_fire_flag_;
    }

    // 发生切换后，在单个信号时长内发送flag
    if (trans_delay >= fire_after_trans_delay_ && fire_delay < fire_flag_keep_delay_) {
        if (!is_fire_flag_) t_fire_ = now;
        is_fire_flag_ = true;
        return is_fire_flag_;
    }

    // 发生切换后，超过单个信号时长，不发送flag
    if (trans_delay >= fire_after_trans_delay_ && fire_delay >= fire_flag_keep_delay_) {
        is_fire_flag_ = false;
        return is_fire_flag_;
    }

    return is_fire_flag_;
}

void RuneV2::setSmallMatrixQ(double q0, double q1, double q2, double q3, double q4, double q5) {
    small_model_.Q << q0, 0, 0, 0, 0, 0,
                      0, q1, 0, 0, 0, 0,
                      0, 0, q2, 0, 0, 0,
                      0, 0, 0, q3, 0, 0,
                      0, 0, 0, 0, q4, 0,
                      0, 0, 0, 0, 0, q5;
}

void RuneV2::setSmallMatrixR(double r0, double r1, double r2, double r3, double r4) {
    small_model_.R << r0, 0, 0, 0, 0,
                      0, r1, 0, 0, 0,
                      0, 0, r2, 0, 0,
                      0, 0, 0, r3, 0,
                      0, 0, 0, 0, r4;
}

void RuneV2::setBigMatrixQ(double q0, double q1, double q2, double q3, double q4, double q5, double q6, double q7) {
    big_model_.Q << q0, 0, 0, 0, 0, 0, 0, 0,
                    0, q1, 0, 0, 0, 0, 0, 0,
                    0, 0, q2, 0, 0, 0, 0, 0,
                    0, 0, 0, q3, 0, 0, 0, 0,
                    0, 0, 0, 0, q4, 0, 0, 0,
                    0, 0, 0, 0, 0, q5, 0, 0,
                    0, 0, 0, 0, 0, 0, q6, 0,
                    0, 0, 0, 0, 0, 0, 0, q7;
}

void RuneV2::setBigMatrixR(double r0, double r1, double r2, double r3, double r4) {
    big_model_.R << r0, 0, 0, 0, 0,
                    0, r1, 0, 0, 0,
                    0, 0, r2, 0, 0,
                    0, 0, 0, r3, 0,
                    0, 0, 0, 0, r4;
}

void RuneV2::setSpdMatrixQ(double q0, double q1) {
    spd_model_.Q << q0, 0,
                    0, q1;
}

void RuneV2::setSpdMatrixR(double r0) {
    spd_model_.R << r0;
}