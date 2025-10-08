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



// AntitopV3 class
// Uses Extended Kalman Filter based center prediction model
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
    double getSafeSub(const double, const double);                  // Safe subtraction for angles
    double getAngleTrans(const double, const double);               // Convert angle in model to approach new angle
    double getAngleTrans(const double, const double, double);       // Convert angle in model to approach new angle, considering prediction
    double getAngleMin(const double, const double, const double);   // Get minimum angle
    int    getToggle(const double, const double);                   // Get toggle label
    double getWeightByTheta(const double);                          // Get weight based on angle
    bool   isAngleTrans(const double, const double);                

    double   r_[2] = {0.25, 0.25};                                  // Radius of two poses
    double   z_[2] = {0, 0};                                        // Height of two poses

    double   r_min_ = 0.15;                                         // Minimum radius
    double   r_max_ = 0.4;                                          // Maximum radius

    int      fire_update_ = 100;                                    // Fire update count
    double   fire_delay_ = 0.5;                                     // Maximum delay considered for model availability
    double   fire_armor_angle_ = 0.5;                               // Fire angle in follow mode
    double   fire_center_angle_ = 0.2;                              // Fire angle for armor plate in center mode
    
    int      toggle_ = 0;                                           // Toggle label
    int      armor_num_ = 4;                                        // Number of armor plates
    int      update_num_ = 0;                                       // Update count

    bool     enable_weighted_ = false;                              // Whether to use weighted average z value
    
    EKF<9, 4>              model_;                                  // Motion model
    KF<4, 2>               center_model_;                           // Center model
    KF<3, 1>               omega_model_;                            // Angular velocity model

    SlideWeightedAvg<double>* weighted_z_;                          // Weighted average z value
    
    AntitopV3_FuncA        funcA_;                                  // State transition function of motion model
    AntitopV3_FuncH        funcH_;                                  // Observation function of motion model
    AntitopV3_CenterFuncA  center_funcA_;                           // State transition function of center model
    AntitopV3_CenterFuncH  center_funcH_;                           // Observation function of center model
    AntitopV3_OmegaFuncA   omega_funcA_;                            // State transition function of angular velocity model
    AntitopV3_OmegaFuncH   omega_funcH_;                            // Observation function of angular velocity model

    TimePoint t_;                                                   // Last update time
};

}

#endif