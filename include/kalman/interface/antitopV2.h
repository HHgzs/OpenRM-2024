#ifndef __OPENRM_KALMAN_INTERFACE_ANTITOP_V2_H__
#define __OPENRM_KALMAN_INTERFACE_ANTITOP_V2_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <structure/slidestd.hpp>
#include <algorithm>

// [ x, y, z, theta, vx, vy, vz, omega, r ]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5,  6,    7,   8 ]  [ 0, 1, 2,   3  ]


namespace rm {

struct AntitopV2_FuncA {
    template<class T>
    void operator()(const T x0[9], T x1[9]) {
        x1[0] = x0[0] + dt * x0[4];
        x1[1] = x0[1] + dt * x0[5];
        x1[2] = x0[2] + dt * x0[6];
        x1[3] = x0[3] + dt * x0[7];
        x1[4] = x0[4];
        x1[5] = x0[5];
        x1[6] = x0[6];
        x1[7] = x0[7];
        x1[8] = x0[8];
    }
    double dt;
};

struct AntitopV2_FuncH {
    template<typename T>
    void operator()(const T x[9], T y[4]) {
        y[0] = x[0] - x[8] * ceres::cos(x[3]);
        y[1] = x[1] - x[8] * ceres::sin(x[3]);
        y[2] = x[2];
        y[3] = x[3];
    }
};


// AntitopV1类
// 使用基于扩展卡尔曼的中心预测模型
class AntitopV2 {

public:
    AntitopV2();
    AntitopV2(double r_min, double r_max, int armor_num = 4);
    ~AntitopV2() {}

    void push(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    Eigen::Matrix<double, 4, 1> getPose(double append_delay);
    Eigen::Matrix<double, 4, 1> getCenter(double append_delay);

    void setMatrixQ(double, double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    void setRadiusRange(double r_min, double r_max);
    void setArmorNum(int armor_num);
    void setFireValue(int update_num, double delay, double armor_angle, double center_angle);

    double getOmega() { return model_.estimate_X[7];};
    void   getStateStr(std::vector<std::string>& str); 
    bool   getFireArmor(const Eigen::Matrix<double, 4, 1>& pose);
    bool   getFireCenter(const Eigen::Matrix<double, 4, 1>& pose);

private:
    double getAngleTrans(const double, const double);               // 将模型内角度转换为接近新角度
    double getAngleTrans(const double, const double, double);       // 将模型预测角度转换为接近新角度,同步转换模型内角度
    double getAngleMin(const double, const double, const double);   // 获取角度最小值
    int    getToggle(const double, const double);                   // 获取切换标签

    double   r_[2] = {0.25, 0.25};                                  // 两个位姿的半径
    double   z_[2] = {0, 0};                                        // 两个位姿的高度

    double   r_min_ = 0.15;                                         // 最小半径
    double   r_max_ = 0.4;                                          // 最大半径

    uint64_t fire_update_ = 100;                                    // 开火更新次数
    double   fire_delay_ = 0.5;                                     // 认为模型可用的最大延迟
    double   fire_angle_ = 0.5;                                     // 跟随模式开火角度
    double   fire_center_angle_ = 0.2;                              // 中心模式装甲板开火角度
    
    int      toggle_ = 0;                                           // 切换标签
    int      armor_num_ = 4;                                        // 装甲板数量
    uint64_t update_num_ = 0;                                       // 更新次数
    
    EKF<9, 4>        model_;                                        // 运动模型
    AntitopV2_FuncA  funcA_;                                        // 运动模型的状态转移函数
    AntitopV2_FuncH  funcH_;                                        // 运动模型的观测函数

    TimePoint t_;                                                   // 上一次更新的时间
};

}

#endif