#ifndef __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V2_H__
#define __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V2_H__
#include <memory>
#include <vector>
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <structure/slidestd.hpp>

// [ x, y, z, theta, vx, vy, vz, omega, ax, ay, b  ]  [ x, y, z, theta ]
// [ 0, 1, 2,   3,   4,  5,  6,    7,   8,  9,  10 ]  [ 0, 1, 2,   3   ]

namespace rm {

struct TrackQueueV2_FuncA {
    template<class T>
    void operator()(const T x0[11], T x1[11]) {
        x1[0] = x0[0] + dt * x0[4] + 0.5 * x0[8] * dt * dt;
        x1[1] = x0[1] + dt * x0[5] + 0.5 * x0[9] * dt * dt;
        x1[2] = x0[2] + dt * x0[6];
        x1[3] = x0[3] + dt * x0[7] + 0.5 * x0[10] * dt * dt;
        x1[4] = x0[4] + dt * x0[8];
        x1[5] = x0[5] + dt * x0[9];
        x1[6] = x0[6];
        x1[7] = x0[7] + dt * x0[10];
        x1[8] = x0[8];
        x1[9] = x0[9];
    }
    double dt;
};

struct TrackQueueV2_FuncH {
    template<typename T>
    void operator()(const T x[11], T y[4]) {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
        y[3] = x[3];
    }
};

class TQstateV2 {
public:
    TimePoint last_t;                       // 目标上一次的时间
    Eigen::Matrix<double, 4, 1> last_pose;  // 目标上一次的位置
    EKF<11, 4> *model;                      // 目标运动模型
    SlideStd<double> *v_std;                // 目标运动模型的速度标准差
    SlideStd<double> *a_std;                // 目标运动模型的加速度标准差
    SlideStd<double> *w_std;                // 目标运动模型的角速度标准差
    int count;                              // 此目标更新计数
    int keep;                               // 此目标保持计数
    bool exist;                             // 目标是否存在
    bool available;                         // 目标信息可用

    TQstateV2() :
        last_t(getTime()),
        count(0),
        keep(5),
        exist(false),
        available(false) {
        model = new EKF<11, 4>();
        v_std = new SlideStd<double>(5);
        a_std = new SlideStd<double>(5);
        w_std = new SlideStd<double>(5);
    }

    void clear() {
        this->count = 0;
        this->keep = 5;
        this->exist = false;
        this->available = false;
        this->model->restart();
        this->v_std->clear();
        this->a_std->clear();
        this->w_std->clear();
    }

    void update(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
        this->last_t = t;
        this->last_pose = pose;
        this->count += 1;
        this->keep = 5;
        this->exist = true;
        this->available = true;
    }
};

class TrackQueueV2 {
public:
    TrackQueueV2() {}
    TrackQueueV2(int count, double distance, double delay, double angle_diff, double toggle_angle);
    ~TrackQueueV2() {}

    void push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t);                       // 推入单次目标信息
    void update();                                                                   // 每帧更新一次

public:
    void setCount(int c) { this->count_ = c; }                                       // 设置认为模型可用的最小更新次数
    void setDistance(double d) { this->distance_ = d; }                              // 设置认为是同一个目标的最大移动距离
    void setDelay(double d) { this->delay_ = d; }                                    // 设置模型不重置的最大延迟
    void setAngleDiffer(double d) { this->angle_diff_ = d; }                         // 设置认为是同一目标的最大夹角
    void setToggleAngle(double d) { this->toggle_angle_ = d; }                       // 设置切换目标的与中心连线夹角之差
    void setFireValue(double sv, double sw, double sa, double angle) {
        this->fire_std_v_ = sv;
        this->fire_std_w_ = sw;
        this->fire_std_a_ = sa;
        this->fire_angle_ = angle;
    }

    void setMatrixQ(double, double, double, double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);

    int         getToggle() { return last_toggle_; }                                 // 获取切换标签
    EKF<11, 4>* getModel();                                                          // 获取更新后的模型                                 
    TimePoint   getLastTime();                                                       // 获取上次更新的时间
    void        getStateStr(std::vector<std::string>& str);                          // 获取目标状态信息字符串                                                            

    Eigen::Matrix<double, 4, 1> getPose();                                           // 直接获取上次传入的位姿
    Eigen::Matrix<double, 4, 1> getPose(double delay);                               // 获取根据模型预测的位姿

    bool isStdStable();                                                              // 判断参数拟合标准差是否稳定
    bool isFireValid(const Eigen::Matrix<double, 4, 1>& pose);                       // 判断是否满足开火条件



private:
    double getDistance(
        const Eigen::Matrix<double, 4, 1>& this_pose,
        const Eigen::Matrix<double, 4, 1>& last_pose);                               // 两目标之间的距离
    double getAngleDiff(double angle0, double angle1);                               // 任意两角度数差的绝对值
    double getAngleOffset(const Eigen::Matrix<double, 4, 1>& pose);                  // 目标方向与车身连线的夹角
    int    getMinAngleOffset();                                                      // 获取具有最小夹角的目标索引
    
    
private:
    int    count_        = 10;              // 可维持状态稳定的最小更新次数
    double distance_     = 0.1;             // 可认为是同一个目标的最大移动距离
    double delay_        = 0.3;             // 可认为是同一个目标的最大更新延迟
    double angle_diff_   = 0.5;             // 可认为是同一块板子的最大夹角之差
    double toggle_angle_ = 0.17;            // 切换目标的夹角之差满足条件才可触发切换

    int    last_index_   = -1;              // 上一次更新的输出目标索引
    int    last_toggle_  = 0;               // 上一次更新的切换标签

    double fire_std_v_   = 0.1;             // 开火速度标准差
    double fire_std_w_   = 0.1;             // 开火角速度标准差
    double fire_std_a_   = 0.1;             // 开火加速度标准差
    double fire_angle_   = 0.5;             // 开火角度

    TrackQueueV2_FuncA funcA_;              // 运动模型的状态转移函数
    TrackQueueV2_FuncH funcH_;              // 运动模型的观测函数

    Eigen::Matrix<double, 11, 11> matrixQ_; // 运动模型的过程噪声协方差矩阵
    Eigen::Matrix<double, 4, 4> matrixR_;   // 运动模型的观测噪声协方差矩阵

public:
    std::vector<TQstateV2> list_;          // 目标状态列表
};

}

#endif