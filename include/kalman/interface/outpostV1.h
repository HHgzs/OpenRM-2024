#ifndef __OPENRM_KALMAN_INTERFACE_OUTPOST_V1_H__
#define __OPENRM_KALMAN_INTERFACE_OUTPOST_V1_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <kalman/filter/kf.h>
#include <structure/slidestd.hpp>
#include <structure/slideweighted.hpp>
#include <algorithm>

// [ x, y, z, theta， omega ]  [ x, y, z, theta]
// [ 0, 1, 2,   3,      4   ]  [ 0, 1, 2,   3  ]

// [ theta, omega ]   [ theta ]
// [   0  ,   1   ]   [   0   ]

namespace rm  {

constexpr double OUTPOST_OMEGA = 0.8 * M_PI;
constexpr double OUTPOST_R = 0.2765;


struct OutpostV1_FuncA {
    template<class T>
    void operator()(const T x0[5], T x1[5]) {
        x1[0] = x0[0];
        x1[1] = x0[1];
        x1[2] = x0[2];
        x1[3] = x0[3] + dt * x0[4];
        x1[4] = x0[4];
    }
    double dt;
};

struct OutpostV1_FuncH {
    template<typename T>
    void operator()(const T x[5], T y[4]) {
        y[0] = x[0] - OUTPOST_R * ceres::cos(x[3]);
        y[1] = x[1] - OUTPOST_R * ceres::sin(x[3]);
        y[2] = x[2];
        y[3] = x[3];
    }
};

struct OutpostV1_OmegaFuncA {
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 1) = dt;
    }
};

struct OutpostV1_OmegaFuncH {
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
    }
};


class OutpostV1 {

public:
    OutpostV1(bool enable_weighted = false);
    ~OutpostV1() {}

    void push(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    Eigen::Matrix<double, 4, 1> getPose(double append_delay);
    Eigen::Matrix<double, 4, 1> getCenter(double append_delay);

    void setMatrixQ(double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    void setMatrixOmegaQ(double, double);
    void setMatrixOmegaR(double);
    void setFireValue(int update_num, double delay, double armor_angle, double center_angle) {
        fire_update_ = update_num;
        fire_delay_ = delay;
        fire_angle_armor_ = armor_angle;
        fire_angle_center_ = center_angle;
    }

    double getOmega() { return model_.estimate_X[4];};
    void   getStateStr(std::vector<std::string>& str); 
    bool   getFireArmor(const Eigen::Matrix<double, 4, 1>& pose);
    bool   getFireCenter(const Eigen::Matrix<double, 4, 1>& pose);

private:
    double getSafeSub(const double, const double);                  // 安全减法
    double getAngleTrans(const double, const double);               // 将模型内角度转换为接近新角度
    double getAngleMin(const double, const double, const double);   // 获取角度最小值
    int    getToggle(const double, const double);                   // 获取切换标签
    double getWeightByTheta(const double);                          // 根据角度获取权重
    bool   isAngleTrans(const double, const double);                // 根据角度确定是否发生切换


    int    fire_update_ = 100;
    double fire_delay_ = 1.5;
    double fire_angle_armor_ = 0.2;
    double fire_angle_center_ = 0.2;


    int toggle_ = 0;
    int update_num_ = 0;

    bool     enable_weighted_ = false;                              // 是否使用加权平均z值

    EKF<5, 4>       model_;
    KF<2, 1>        omega_model_;

    OutpostV1_FuncA funcA_;
    OutpostV1_FuncH funcH_;

    OutpostV1_OmegaFuncA omega_funcA_;
    OutpostV1_OmegaFuncH omega_funcH_;

    TimePoint t_;
    SlideAvg<double> center_x_;
    SlideAvg<double> center_y_;
    SlideAvg<double> center_z_;
    SlideAvg<double> omega_;
    SlideWeightedAvg<double> weighted_z_;
};


}






#endif