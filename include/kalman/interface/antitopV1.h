#ifndef __OPENRM_KALMAN_INTERFACE_ANTITOP_V1_H__
#define __OPENRM_KALMAN_INTERFACE_ANTITOP_V1_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <structure/slidestd.hpp>
#include <algorithm>

// [ x, y, z, theta, vx, vy, vz, omega, r ]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5,  6,    7,   8 ]  [ 0, 1, 2,   3  ]


namespace rm {

struct AntitopV1_FuncA {
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

struct AntitopV1_FuncH {
    template<typename T>
    void operator()(const T x[9], T y[4]) {
        y[0] = x[0] - x[8] * ceres::cos(x[3]);
        y[1] = x[1] - x[8] * ceres::sin(x[3]);
        y[2] = x[2];
        y[3] = x[3];
    }
};


// AntitopV1 class
// Uses Extended Kalman Filter based center prediction model
class AntitopV1 {

public:
    AntitopV1();
    AntitopV1(double r_min, double r_max, int armor_num = 4);
    ~AntitopV1() {}

    void push(const Eigen::Matrix<double, 4, 1>& pose, const TimePoint& t, int toggle = 0);
    Eigen::Matrix<double, 4, 1> getPose(double delay);
    void setMatrixQ(double, double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    void setRadiusRange(double r_min, double r_max);
    void setArmorNum(int armor_num);
    void setStdValue(double std_v, double std_w);
    void setFireValue(double angle, int update_num);

    double getOmega() { return model_.estimate_X[7];};
    double getStdV() { return slidestd_v_.getStd();};
    double getStdW() { return slidestd_w_.getStd();};

    bool isStdStable();
    bool isFireValid(const Eigen::Matrix<double, 4, 1>& pose);



private:
    double getAngleTrans(const double, const double);               // Convert angle in model to approach new angle
    double getAngleTrans(const double, const double, double);       // Convert predicted angle in model to approach new angle, synchronously convert angle in model
    double getAngleMin(const double, const double, const double);   // Get minimum angle
    int    getToggle(const double, const double);                   // Get toggle label

    double   r_[2] = {0.25, 0.25};                                  // Radius of two poses
    double   z_[2] = {0, 0};                                        // Height of two poses

    double   r_min_ = 0.15;                                         // Minimum radius
    double   r_max_ = 0.4;                                          // Maximum radius

    double   fire_std_v_ = 0.1;                                     // Fire velocity standard deviation
    double   fire_std_w_ = 0.1;                                     // Fire angular velocity standard deviation
    double   fire_angle_ = 0.75;                                    // Fire angle
    uint64_t fire_update_ = 50;                                     // Fire update count

    int      toggle_ = 0;                                           // Toggle label
    int      armor_num_ = 4;                                        // Number of armor plates
    uint64_t update_num_ = 0;                                       // Update count
    
    EKF<9, 4>        model_;                                        // Motion model
    AntitopV1_FuncA  funcA_;                                        // State transition function of motion model
    AntitopV1_FuncH  funcH_;                                        // Observation function of motion model

    TimePoint t_;                                                   // Last update time
    SlideStd<double> slidestd_v_;                                   // Velocity standard deviation
    SlideStd<double> slidestd_w_;                                   // Angular velocity standard deviation
};

}

#endif