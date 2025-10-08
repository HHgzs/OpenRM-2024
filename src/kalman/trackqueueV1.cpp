#include "kalman/interface/trackqueueV1.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>

#include "utils/print.h"

using namespace rm;
using namespace std;

// [ x, y, z, theta, vx, vy]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5 ]  [ 0, 1, 2,   3  ]

TrackQueueV1::TrackQueueV1() {
    list_.resize(4);
    for(int i = 0; i < list_.size(); i++) {
        list_[i] = TQstateV1();
    }
    setMatrixQ(0.2, 0.2, 0.1, 0.01, 1.0, 1.0);
    setMatrixR(0.001, 0.001, 0.1, 0.1);
}

TrackQueueV1::TrackQueueV1(int min_count, double max_distance, double max_delay, double toggle_angle, double max_std):
    min_count_(min_count),
    max_distance_(max_distance),
    max_delay_(max_delay),
    toggle_angle_offset_(toggle_angle),
    max_std_(max_std) {

    list_.resize(4);
    for(int i = 0; i < list_.size(); i++) {
        list_[i] = TQstateV1();
    }

    setMatrixQ(0.2, 0.2, 0.1, 0.01, 1.0, 1.0);
    setMatrixR(0.001, 0.001, 0.1, 0.1);
}

void TrackQueueV1::push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
    double min_distance = 10000.0;
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
        if (!isDelayValid(list_[i].last_t, t) || list_[i].count < 0) {
            list_[i].clear();
            if(last_index_ == i) {
                last_index_ = -1;
            }
            continue;
        }

        // 把所有的目标lose都减一
        list_[i].count--;

        double d = getDistance(pose, list_[i].last_pose);
        if (d < min_distance) {
            min_distance = d;
            min_index = i;
        }
    }

    // 找到了最佳预测目标
    if(min_index != -1 && isDistanceValid(min_distance)) {
        double dt = getDoubleOfS(list_[min_index].last_t, t);
        list_[min_index].update(pose, t);

        funcA_.dt = dt;
        list_[min_index].model->predict(funcA_);
        list_[min_index].model->update(funcH_, pose);
        
        double v = sqrt(list_[min_index].model->estimate_X[4] * list_[min_index].model->estimate_X[4] + 
                        list_[min_index].model->estimate_X[5] * list_[min_index].model->estimate_X[5]);
        list_[min_index].v_std->push(v);

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
            list_[first_empty].v_std->push(v);

            return;

        // 没有空目标，新建一个目标
        } else {
            list_.push_back(TQstateV1());
            list_[list_.size() - 1].model->Q = matrixQ_;
            list_[list_.size() - 1].model->R = matrixR_;
            list_[list_.size() - 1].update(pose, t);

            list_[list_.size() - 1].model->restart();
            funcA_.dt = 0.0;
            list_[list_.size() - 1].model->predict(funcA_);
            list_[list_.size() - 1].model->update(funcH_, pose);

            double v = sqrt(list_[list_.size() - 1].model->estimate_X[4] * list_[list_.size() - 1].model->estimate_X[4] + 
                            list_[list_.size() - 1].model->estimate_X[5] * list_[list_.size() - 1].model->estimate_X[5]);
            list_[list_.size() - 1].v_std->push(v);

            return;
        }
    }
}

void TrackQueueV1::update() {
    int min_angle_index = getMinAngleOffset();

    if ((last_index_ == -1) || !list_[last_index_].exist) {
        last_toggle_ = !last_toggle_;
        last_index_ = min_angle_index;
        return;
    } else if (
        last_index_ != min_angle_index && 
        isCountUseValid(list_[min_angle_index].count) &&
        isAngleOffsetValid(
            getAngleOffset(list_[last_index_].last_pose), 
            getAngleOffset(list_[min_angle_index].last_pose))
    ) {
        last_index_ = min_angle_index;
        last_toggle_ = !last_toggle_;
        return;
    }
}

void TrackQueueV1::setMatrixQ(double q1, double q2, double q3, double q4, double q5, double q6) {
    matrixQ_ << q1, 0, 0, 0, 0, 0,
                0, q2, 0, 0, 0, 0,
                0, 0, q3, 0, 0, 0,
                0, 0, 0, q4, 0, 0,
                0, 0, 0, 0, q5, 0,
                0, 0, 0, 0, 0, q6;
    for(int i = 0; i < list_.size(); i++) {
        list_[i].model->Q = matrixQ_;
    }
}

void TrackQueueV1::setMatrixR(double r1, double r2, double r3, double r4) {
    matrixR_ << r1, 0, 0, 0,
                0, r2, 0, 0,
                0, 0, r3, 0,
                0, 0, 0, r4;
    for(int i = 0; i < list_.size(); i++) {
        list_[i].model->R = matrixR_;
    }
}

KF<6, 4>* TrackQueueV1::getModel() {
    if (last_index_ == -1) {
        return nullptr;
    }
    return list_[last_index_].model;
}

TimePoint TrackQueueV1::getLastTime() {
    if ((last_index_ == -1) || !list_[last_index_].exist || !list_[last_index_].available) {
        return getTime();
    }
    return list_[last_index_].last_t;
}

double TrackQueueV1::getStd() {
    if ((last_index_ == -1) || !list_[last_index_].exist) {
        return -1;
    }
    return list_[last_index_].v_std->getStd();
}

bool TrackQueueV1::isStdValid() {
    if ((last_index_ == -1) || !list_[last_index_].exist) {
        return false;
    }
    if (list_[last_index_].v_std->getStd() > this->max_std_) {
        return false;
    }
    return true;
}

Eigen::Matrix<double, 4, 1> TrackQueueV1::getPose() {
    if ((last_index_ == -1) || !list_[last_index_].exist || !list_[last_index_].available) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    // rm::print8d(
    //     (double)last_toggle_,
    //     (double)last_index_,
    //     list_[last_index_].v_std->getStd(),
    //     (double)isStdValid(),
    //     list_[last_index_].model->estimate_X[0],
    //     list_[last_index_].model->estimate_X[1],
    //     list_[last_index_].model->estimate_X[4],
    //     list_[last_index_].model->estimate_X[5],
    //     "toggle", "index", "std", "valid", "x", "y", "vx", "vy"
    // );

    list_[last_index_].available = false;
    return list_[last_index_].last_pose;
}

Eigen::Matrix<double, 4, 1> TrackQueueV1::getPose(double delay) {
    if ((last_index_ == -1) || !list_[last_index_].exist) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    TimePoint now = getTime();
    double dt = getDoubleOfS(list_[last_index_].last_t, now) + delay;

    double x = list_[last_index_].model->estimate_X[0] + dt * list_[last_index_].model->estimate_X[4];
    double y = list_[last_index_].model->estimate_X[1] + dt * list_[last_index_].model->estimate_X[5];
    double z = list_[last_index_].model->estimate_X[2];
    double theta = list_[last_index_].model->estimate_X[3];
    Eigen::Matrix<double, 4, 1> pose;
    pose << x, y, z, theta;
    return pose;
}

double TrackQueueV1::getAngleOffset(const Eigen::Matrix<double, 4, 1>& pose) {
    double angle_pose = atan2(pose(1), pose(0));
    double angle_offset = angle_pose - pose[3];
    if (angle_offset > M_PI) {
        angle_offset -= 2 * M_PI;
    } else if (angle_offset < -M_PI) {
        angle_offset += 2 * M_PI;
    }
    return abs(angle_offset);
}

int TrackQueueV1::getMinAngleOffset() {
    int min_index = -1;
    double min_angle_offset = 10000.0;
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

double TrackQueueV1::getDistance(const Eigen::Matrix<double, 4, 1>& this_pose, const Eigen::Matrix<double, 4, 1>& last_pose) {
    double dx = this_pose(0) - last_pose(0);
    double dy = this_pose(1) - last_pose(1);
    double dz = this_pose(2) - last_pose(2);
    double d = std::sqrt(dx * dx + dy * dy + dz * dz);
    return d;
}

bool TrackQueueV1::isDistanceValid(double d) {
    if (d > this->max_distance_) {
        return false;
    }
    return true;
}

bool TrackQueueV1::isDelayValid(const TimePoint& last_t, const TimePoint& this_t) {
    double dt = getDoubleOfS(last_t, this_t);
    if (dt > this->max_delay_) {
        return false;
    }
    return true;
}

bool TrackQueueV1::isCountKeepValid(int count) {
    if (count < 0) {
        return false;
    }
    return true;
}

bool TrackQueueV1::isCountUseValid(int count) {
    if (count < this->min_count_) {
        return false;
    }
    return true;
}

bool TrackQueueV1::isAngleOffsetValid(double angle0, double angle1) {
    if (abs(angle0 - angle1) < this->toggle_angle_offset_) {
        return false;
    }
    return true;
}