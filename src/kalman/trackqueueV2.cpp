#include "kalman/interface/trackqueueV2.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>

#include "utils/print.h"

using namespace rm;
using namespace std;

// [ x, y, z, theta, vx, vy, vz, omega, ax, ay, b ]  [ x, y, z, theta ]
// [ 0, 1, 2,   3,   4,  5,  6,    7,   8,  9, 10 ]  [ 0, 1, 2,   3   ]

TrackQueueV2::TrackQueueV2(int count, double distance, double delay, double angle_diff, double toggle_angle):
    count_(count),
    distance_(distance),
    delay_(delay),
    angle_diff_(angle_diff),
    toggle_angle_(toggle_angle) {

    list_.resize(4);
    for(int i = 0; i < list_.size(); i++) {
        list_[i] = TQstateV2();
    }

    setMatrixQ(0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
    setMatrixR(0.1, 0.1, 0.1, 0.1);
}

void TrackQueueV2::push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
    double min_distance = 1e4;
    int min_index = -1;
    int first_empty = -1;

    // 先更新所有目标，查找最佳预测目标
    for(int i = 0; i < list_.size(); i++) {
        if (!list_[i].exist) {
            if(first_empty == -1) {
                first_empty = i;
            }
            continue;
        }
        if ((getDoubleOfS(list_[i].last_t, t) > delay_) || (list_[i].keep <= 0)) {
            list_[i].clear();
            if(last_index_ == i) {
                last_index_ = -1;
            }
            continue;
        }

        // 把所有的目标keep都减一
        list_[i].keep--;

        double dt = getDoubleOfS(list_[i].last_t, t);
        double predict_x = list_[i].model->estimate_X[0] + dt * list_[i].model->estimate_X[4];
        double predict_y = list_[i].model->estimate_X[1] + dt * list_[i].model->estimate_X[5];
        double predict_z = list_[i].model->estimate_X[2];
        double predict_theta = list_[i].model->estimate_X[3] + dt * list_[i].model->estimate_X[7];
        Eigen::Matrix<double, 4, 1> predict_pose;
        predict_pose << predict_x, predict_y, predict_z, predict_theta;
        
        double d = getDistance(pose, predict_pose);
        double angle_diff = abs(predict_theta - pose[3]);
        
        if ((d < distance_) && (angle_diff < angle_diff_) && (d < min_distance)) {
            min_distance = d;
            min_index = i;
        }
    }

    // 找到了最佳预测目标
    if((min_index != -1) && (min_distance < distance_)) {
        
        double dt = getDoubleOfS(list_[min_index].last_t, t);
        list_[min_index].update(pose, t);

        funcA_.dt = dt;
        list_[min_index].model->predict(funcA_);
        list_[min_index].model->update(funcH_, pose);
        
        double v = sqrt(list_[min_index].model->estimate_X[4] * list_[min_index].model->estimate_X[4] + 
                        list_[min_index].model->estimate_X[5] * list_[min_index].model->estimate_X[5]);
        double a = sqrt(list_[min_index].model->estimate_X[8] * list_[min_index].model->estimate_X[8] + 
                        list_[min_index].model->estimate_X[9] * list_[min_index].model->estimate_X[9]);
        double w = list_[min_index].model->estimate_X[7];

        list_[min_index].v_std->push(v);
        list_[min_index].a_std->push(a);
        list_[min_index].w_std->push(w);

        return;

    // 没有找到最佳预测目标
    } else {
        // 有空目标，使用空目标
        if(first_empty != -1) {
            list_[first_empty].update(pose, t);
            
            list_[first_empty].model->restart();
            funcA_.dt = 0.0;
            list_[first_empty].model->predict(funcA_);
            list_[first_empty].model->update(funcH_, pose);

            double v = sqrt(list_[first_empty].model->estimate_X[4] * list_[first_empty].model->estimate_X[4] + 
                            list_[first_empty].model->estimate_X[5] * list_[first_empty].model->estimate_X[5]);
            double a = sqrt(list_[first_empty].model->estimate_X[8] * list_[first_empty].model->estimate_X[8] + 
                            list_[first_empty].model->estimate_X[9] * list_[first_empty].model->estimate_X[9]);
            double w = list_[first_empty].model->estimate_X[7];

            list_[first_empty].v_std->push(v);
            list_[first_empty].a_std->push(a);
            list_[first_empty].w_std->push(w);

            return;

        // 没有空目标，新建一个目标
        } else {
            list_.push_back(TQstateV2());
            list_[list_.size() - 1].model->Q = matrixQ_;
            list_[list_.size() - 1].model->R = matrixR_;
            list_[list_.size() - 1].update(pose, t);

            list_[list_.size() - 1].model->restart();
            funcA_.dt = 0.0;
            list_[list_.size() - 1].model->predict(funcA_);
            list_[list_.size() - 1].model->update(funcH_, pose);

            double v = sqrt(list_[list_.size() - 1].model->estimate_X[4] * list_[list_.size() - 1].model->estimate_X[4] + 
                            list_[list_.size() - 1].model->estimate_X[5] * list_[list_.size() - 1].model->estimate_X[5]);
            double a = sqrt(list_[list_.size() - 1].model->estimate_X[8] * list_[list_.size() - 1].model->estimate_X[8] +
                            list_[list_.size() - 1].model->estimate_X[9] * list_[list_.size() - 1].model->estimate_X[9]);
            double w = list_[list_.size() - 1].model->estimate_X[7];
            
            list_[list_.size() - 1].v_std->push(v);
            list_[list_.size() - 1].a_std->push(a);
            list_[list_.size() - 1].w_std->push(w);

            return;
        }
    }
}

void TrackQueueV2::update() {
    int min_angle_index = getMinAngleOffset();

    if ((last_index_ == -1) || !list_[last_index_].exist) {
        last_toggle_ = !last_toggle_;
        last_index_ = min_angle_index;
        return;
    } else if (
        last_index_ != min_angle_index && 
        list_[min_angle_index].count > count_ &&
        getAngleDiff(
            getAngleOffset(list_[last_index_].last_pose),
            getAngleOffset(list_[min_angle_index].last_pose)
        ) > toggle_angle_
    ) {
        last_index_ = min_angle_index;
        last_toggle_ = !last_toggle_;
        return;
    }
}

void TrackQueueV2::setMatrixQ(
        double q1, double q2, double q3, double q4, double q5, double q6,
        double q7, double q8, double q9, double q10, double q11
) {
    matrixQ_ << q1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, q2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, q3, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, q4, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, q5, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, q6, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, q7, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, q8, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, q9, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, q10, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, q11;

    for(int i = 0; i < list_.size(); i++) {
        list_[i].model->Q = matrixQ_;
    }
}

void TrackQueueV2::setMatrixR(double r1, double r2, double r3, double r4) {
    matrixR_ << r1, 0, 0, 0,
                0, r2, 0, 0,
                0, 0, r3, 0,
                0, 0, 0, r4;
    for(int i = 0; i < list_.size(); i++) {
        list_[i].model->R = matrixR_;
    }
}

EKF<11, 4>* TrackQueueV2::getModel() {
    if (last_index_ == -1) {
        return nullptr;
    }
    return list_[last_index_].model;
}

TimePoint TrackQueueV2::getLastTime() {
    if ((last_index_ == -1) || !list_[last_index_].exist || !list_[last_index_].available) {
        return getTime();
    }
    return list_[last_index_].last_t;
}

void TrackQueueV2::getStateStr(std::vector<std::string>& str) {
    str.clear();
    str.push_back("TrackQueueV2:");
    str.push_back("last_index: " + to_string(last_index_));
    str.push_back("last_toggle: " + to_string(last_toggle_));
    str.push_back(" ");
    for(int i = 0; i < list_.size(); i++) {
        if (!list_[i].exist) continue;
        str.push_back("Track " + to_string(i) + ":");
        str.push_back(" count: " + to_string(list_[i].count));
        str.push_back(" keep: " + to_string(list_[i].keep));
        str.push_back(" avail: " + to_string(list_[i].available));
        str.push_back(" ");
    }
}

Eigen::Matrix<double, 4, 1> TrackQueueV2::getPose() {
    if ((last_index_ == -1) || !list_[last_index_].exist || !list_[last_index_].available) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }

    list_[last_index_].available = false;
    return list_[last_index_].last_pose;
}

Eigen::Matrix<double, 4, 1> TrackQueueV2::getPose(double delay) {
    if ((last_index_ == -1) || !list_[last_index_].exist) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    TimePoint now = getTime();
    double sys_delay = getDoubleOfS(list_[last_index_].last_t, now);
    if (sys_delay > 0.5) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }

    double dt = sys_delay + delay;

    double x = list_[last_index_].model->estimate_X[0] + dt * list_[last_index_].model->estimate_X[4];
    double y = list_[last_index_].model->estimate_X[1] + dt * list_[last_index_].model->estimate_X[5];
    double z = list_[last_index_].model->estimate_X[2];
    double theta = list_[last_index_].model->estimate_X[3] + dt * list_[last_index_].model->estimate_X[7];

    Eigen::Matrix<double, 4, 1> pose;
    pose << x, y, z, theta;
    return pose;
}

double TrackQueueV2::getDistance(const Eigen::Matrix<double, 4, 1>& this_pose, const Eigen::Matrix<double, 4, 1>& last_pose) {
    double dx = this_pose(0) - last_pose(0);
    double dy = this_pose(1) - last_pose(1);
    double dz = this_pose(2) - last_pose(2);
    double d = std::sqrt(dx * dx + dy * dy + dz * dz);
    return d;
}

double TrackQueueV2::getAngleDiff(double angle0, double angle1) {
    double angle_diff = angle0 - angle1;
    while(angle_diff > M_PI) angle_diff -= 2 * M_PI;
    while(angle_diff < -M_PI) angle_diff += 2 * M_PI;
    return abs(angle_diff);
}

double TrackQueueV2::getAngleOffset(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle_center = atan2(pose(1), pose(0));
    double angle_offset = getAngleDiff(angle_center, pose[3]);
    return angle_offset;
}

int TrackQueueV2::getMinAngleOffset() {
    int min_index = -1;
    double min_angle_offset = 1e5;
    for(int i = 0; i < list_.size(); i++) {
        if (!list_[i].exist) {
            continue;
        }
        double angle_offset = getAngleOffset(list_[i].last_pose);
        if (angle_offset < min_angle_offset) {
            min_angle_offset = angle_offset;
            min_index = i;
        }
    }
    return min_index;
}

bool TrackQueueV2::isStdStable() {
    if ((last_index_ == -1) || !list_[last_index_].exist) {
        return false;
    }
    double sv = list_[last_index_].v_std->getStd();
    double sw = list_[last_index_].w_std->getStd();
    double sa = list_[last_index_].a_std->getStd();
    if (((sv < fire_std_v_) || (sa < fire_std_a_)) && (sw < fire_std_w_)) {
        return true;
    }
    return false;
}

bool TrackQueueV2::isFireValid(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle_offset = getAngleOffset(pose);
    if(angle_offset < fire_angle_) {
        return true;
    }
    return false;
}