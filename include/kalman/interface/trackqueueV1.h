#ifndef __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V1_H__
#define __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V1_H__
#include <memory>
#include <vector>
#include <utils/timer.h>
#include <kalman/filter/kf.h>
#include <structure/slidestd.hpp>

// [ x, y, z, theta, vx, vy]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5 ]  [ 0, 1, 2,   3  ]

namespace rm {

struct TrackQueueV1_FuncA{
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 4) = dt;
        A(1, 5) = dt;
    }
};

struct TrackQueueV1_FuncH{
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
        H(1, 1) = 1;
        H(2, 2) = 1;
        H(3, 3) = 1;
    }
};

class TQstateV1 {
public:
    TimePoint last_t;                       // 目标上一次的时间
    Eigen::Matrix<double, 4, 1> last_pose;  // 目标上一次的位置
    KF<6, 4> *model;                        // 目标运动模型
    SlideStd<double> *v_std;                // 目标运动模型的速度标准差
    int count;                              // 此目标更新计数
    bool exist;                             // 目标是否存在
    bool available;                         // 目标信息可用

    TQstateV1() :
        last_t(getTime()),
        count(0),
        exist(false),
        available(false) {
        model = new KF<6, 4>();
        v_std = new SlideStd<double>(10);
    }

    void clear() {
        this->count = 0;
        this->exist = false;
        this->available = false;
        this->model->restart();
        this->v_std->clear();
    }

    void update(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
        this->last_t = t;
        this->last_pose = pose;
        this->count += 2;
        this->exist = true;
        this->available = true;
    }
};

class TrackQueueV1 {
public:
    TrackQueueV1();
    TrackQueueV1(int, double, double, double, double);
    ~TrackQueueV1() {}

    void push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    void update();


public:
    void setMinCount(int c) { this->min_count_ = c; }
    void setMaxDistance(double d) { this->max_distance_ = d; }
    void setMaxDelay(double d) { this->max_delay_ = d; }
    void setToggleAngleOffset(double d) { this->toggle_angle_offset_ = d; }
    void setMaxStd(double d) { this->max_std_ = d; }
    void setMatrixQ(double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    int getToggle() { return last_toggle_; }
    KF<6, 4>* getModel();                                                            // 获取更新后的模型                                 
    TimePoint getLastTime();                                                         // 获取上次更新的时间
    double getStd();                                                                 // 获取上次更新目标的速度标准差
    bool isStdValid();                                                               // 根据速度标准差判断是否可用
    Eigen::Matrix<double, 4, 1> getPose();                                           // 直接获取上次更新的位姿
    Eigen::Matrix<double, 4, 1> getPose(double delay);                               // 获取根据模型预测的位姿

private:
    double getAngleOffset(const Eigen::Matrix<double, 4, 1>& pose);                  // 目标指向相对其与车身连线的夹角
    int getMinAngleOffset();                                                                                              // 获取具有最小夹角的目标索引
    double getDistance(const Eigen::Matrix<double, 4, 1>& this_pose, const Eigen::Matrix<double, 4, 1>& last_pose);       // 两目标之间的距离
    bool isDistanceValid(double d);                                                  // 根据移动距离判断是否可认为是同一个目标 
    bool isDelayValid(const TimePoint& this_t, const TimePoint& last_t);             // 根据更新延迟判断是否需要丢弃
    bool isCountKeepValid(int count);                                                // 根据更新计数判断是否可维持状态
    bool isCountUseValid(int count);                                                 // 根据更新计数判断是否可用
    bool isAngleOffsetValid(double angle0, double angle1);                           // 根据夹角之差判断是否可进行切换
    

private:
    int min_count_ = 5;                     // 可维持状态稳定的最小更新次数
    double max_distance_ = 0.1;             // 可认为是同一个目标的最大移动距离
    double max_delay_ = 0.3;                // 可认为是同一个目标的最大更新延迟
    double toggle_angle_offset_ = 0.17;     // 切换目标的夹角之差满足条件才可触发切换
    double max_std_ = 0.1;                  // 速度标准差的最大值

    int last_index_ = -1;                   // 上一次更新的输出目标索引
    int last_toggle_ = 0;                   // 上一次更新的切换标签

    TrackQueueV1_FuncA funcA_;              // 运动模型的状态转移函数
    TrackQueueV1_FuncH funcH_;              // 运动模型的观测函数
    Eigen::Matrix<double, 6, 6> matrixQ_;   // 运动模型的过程噪声协方差矩阵
    Eigen::Matrix<double, 4, 4> matrixR_;   // 运动模型的观测噪声协方差矩阵

public:
    std::vector<TQstateV1> list_;            // 目标状态列表
};

}

#endif