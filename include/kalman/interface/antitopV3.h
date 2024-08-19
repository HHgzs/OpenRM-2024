#ifndef __OPENRM_KALMAN_INTERFACE_ANTITOP_V3_H__
#define __OPENRM_KALMAN_INTERFACE_ANTITOP_V3_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <kalman/filter/kf.h>
#include <structure/slidestd.hpp>
#include <structure/slideweighted.hpp>
#include <algorithm>

// [ x, y, z, theta, vx, vy, vz, omega, r ]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5,  6,    7,   8 ]  [ 0, 1, 2,   3  ]

// [ x, y, vx, vy ]    [ x, y ]
// [ 0, 1, 2,  3  ]    [ 0, 1 ]

// [ theta, omega, beta ]    [ theta ]
// [   0,     1,    2   ]    [   0   ]

namespace rm {

struct AntitopV3_FuncA {
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

struct AntitopV3_FuncH {
    template<typename T>
    void operator()(const T x[9], T y[4]) {
        y[0] = x[0] - x[8] * ceres::cos(x[3]);
        y[1] = x[1] - x[8] * ceres::sin(x[3]);
        y[2] = x[2];
        y[3] = x[3];
    }
};

struct AntitopV3_CenterFuncA{
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 2) = dt;
        A(1, 3) = dt;
    }
};

struct AntitopV3_CenterFuncH{
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
        H(1, 1) = 1;
    }
};

struct AntitopV3_OmegaFuncA {
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 1) = dt;
        A(1, 2) = dt;
        A(0, 2) = dt * dt;
    }
};

struct AntitopV3_OmegaFuncH {
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
    }
};



// AntitopV1类
// 使用基于扩展卡尔曼的中心预测模型
class AntitopV3 {

public:
    AntitopV3();
    AntitopV3(double r_min, double r_max, int armor_num = 4, bool enable_weighted = false);
    ~AntitopV3() {}

    void push(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    Eigen::Matrix<double, 4, 1> getPose(double append_delay);
    Eigen::Matrix<double, 4, 1> getCenter(double append_delay);

    void setMatrixQ(double, double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    void setCenterMatrixQ(double, double, double, double);
    void setCenterMatrixR(double, double);
    void setOmegaMatrixQ(double, double, double);
    void setOmegaMatrixR(double);
    void setRadiusRange(double r_min, double r_max) { r_min_ = r_min; r_max_ = r_max; }
    void setArmorNum(int armor_num) { armor_num_ = armor_num; }
    void setFireValue(int update_num, double delay, double armor_angle, double center_angle) {
        fire_update_ = update_num;
        fire_delay_ = delay;
        fire_armor_angle_ = armor_angle;
        fire_center_angle_ = center_angle;
    }

    double getOmega() { return omega_model_.estimate_X[1];};
    void   getStateStr(std::vector<std::string>& str); 
    bool   getFireArmor(const Eigen::Matrix<double, 4, 1>& pose);
    bool   getFireCenter(const Eigen::Matrix<double, 4, 1>& pose);

private:
    double getSafeSub(const double, const double);                  // 角度安全减法
    double getAngleTrans(const double, const double);               // 将模型内角度转换为接近新角度
    double getAngleTrans(const double, const double, double);       // 将模型内角度转换为接近新角度，转换考虑预测
    double getAngleMin(const double, const double, const double);   // 获取角度最小值
    int    getToggle(const double, const double);                   // 获取切换标签
    double getWeightByTheta(const double);                          // 根据角度获取权重
    bool   isAngleTrans(const double, const double);                

    double   r_[2] = {0.25, 0.25};                                  // 两个位姿的半径
    double   z_[2] = {0, 0};                                        // 两个位姿的高度

    double   r_min_ = 0.15;                                         // 最小半径
    double   r_max_ = 0.4;                                          // 最大半径

    int      fire_update_ = 100;                                    // 开火更新次数
    double   fire_delay_ = 0.5;                                     // 认为模型可用的最大延迟
    double   fire_armor_angle_ = 0.5;                               // 跟随模式开火角度
    double   fire_center_angle_ = 0.2;                              // 中心模式装甲板开火角度
    
    int      toggle_ = 0;                                           // 切换标签
    int      armor_num_ = 4;                                        // 装甲板数量
    int      update_num_ = 0;                                       // 更新次数

    bool     enable_weighted_ = false;                              // 是否使用加权平均z值
    
    EKF<9, 4>              model_;                                  // 运动模型
    KF<4, 2>               center_model_;                           // 中心模型
    KF<3, 1>               omega_model_;                            // 角速度模型

    SlideWeightedAvg<double>* weighted_z_;                          // z值加权平均
    
    AntitopV3_FuncA        funcA_;                                  // 运动模型的状态转移函数
    AntitopV3_FuncH        funcH_;                                  // 运动模型的观测函数
    AntitopV3_CenterFuncA  center_funcA_;                           // 中心模型的状态转移函数
    AntitopV3_CenterFuncH  center_funcH_;                           // 中心模型的观测函数
    AntitopV3_OmegaFuncA   omega_funcA_;                            // 角速度模型的状态转移函数
    AntitopV3_OmegaFuncH   omega_funcH_;                            // 角速度模型的观测函数

    TimePoint t_;                                                   // 上一次更新的时间
};

}

#endif