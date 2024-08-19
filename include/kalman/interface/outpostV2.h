#ifndef __OPENRM_KALMAN_INTERFACE_OUTPOST_V2_H__
#define __OPENRM_KALMAN_INTERFACE_OUTPOST_V2_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <kalman/filter/kf.h>
#include <structure/slidestd.hpp>
#include <algorithm>

// [ x, y, z, theta, vx, vy, vz, omega ]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5,  6,    7   ]  [ 0, 1, 2,   3  ]

// [ theta, omega ]   [ theta ]
// [   0  ,   1   ]   [   0   ]

namespace rm  {

constexpr double OUTPOST_OMEGA_V2 = 0.8 * M_PI;
constexpr double OUTPOST_R_V2 = 0.2765;


struct OutpostV2_FuncA {
    template<class T>
    void operator()(const T x0[8], T x1[5]) {
        x1[0] = x0[0] + dt * x0[4];
        x1[1] = x0[1] + dt * x0[5];
        x1[2] = x0[2] + dt * x0[6];
        x1[3] = x0[3] + dt * x0[7];
        x1[4] = x0[4];
        x1[5] = x0[5];
        x1[6] = x0[6];
        x1[7] = x0[7];
    }
    double dt;
};

struct OutpostV2_FuncH {
    template<typename T>
    void operator()(const T x[8], T y[4]) {
        y[0] = x[0] - OUTPOST_R_V2 * ceres::cos(x[3]);
        y[1] = x[1] - OUTPOST_R_V2 * ceres::sin(x[3]);
        y[2] = x[2];
        y[3] = x[3];
    }
};

struct OutpostV2_OmegaFuncA {
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 1) = dt;
    }
};

struct OutpostV2_OmegaFuncH {
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
    }
};


class OutpostV2 {

public:
    OutpostV2();
    ~OutpostV2() {}

    void push(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    Eigen::Matrix<double, 4, 1> getPose(double append_delay);
    Eigen::Matrix<double, 4, 1> getCenter(double append_delay);

    void setMatrixQ(double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    void setMatrixOmegaQ(double, double);
    void setMatrixOmegaR(double);
    void setFireValue(int update_num, double delay, double armor_angle, double center_angle) {
        fire_update_ = update_num;
        fire_delay_ = delay;
        fire_angle_armor_ = armor_angle;
        fire_angle_center_ = center_angle;
    }

    double getOmega() { return model_.estimate_X[7];};
    void   getStateStr(std::vector<std::string>& str); 
    bool   getFireArmor(const Eigen::Matrix<double, 4, 1>& pose);
    bool   getFireCenter(const Eigen::Matrix<double, 4, 1>& pose);

private:
    double getSafeSub(const double, const double);                  // 安全减法
    double getAngleTrans(const double, const double);               // 将模型内角度转换为接近新角度
    double getAngleMin(const double, const double, const double);   // 获取角度最小值
    int    getToggle(const double, const double);                   // 获取切换标签
    bool   isAngleTrans(const double, const double);                // 根据角度确定是否发生切换


    int    fire_update_ = 100;
    double fire_delay_ = 1.5;
    double fire_angle_armor_ = 0.2;
    double fire_angle_center_ = 0.2;


    int toggle_ = 0;
    int update_num_ = 0;

    EKF<8, 4>       model_;
    KF<2, 1>        omega_model_;

    OutpostV2_FuncA funcA_;
    OutpostV2_FuncH funcH_;

    OutpostV2_OmegaFuncA omega_funcA_;
    OutpostV2_OmegaFuncH omega_funcH_;

    TimePoint t_;
    SlideAvg<double> omega_;
};


}






#endif