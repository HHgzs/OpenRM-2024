#ifndef __OPENRM_KALMAN_INTERFACE_RUNE_V2_H__
#define __OPENRM_KALMAN_INTERFACE_RUNE_V2_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <kalman/filter/kf.h>
#include <structure/slidestd.hpp>
#include <algorithm>

// a in [0.780, 1.045]
// w in [1.884, 2.000]
// b = 2.090 - a

// x, y, z:    Center point coordinates of rune in prediction sequence, target center coordinates in observation sequence
// theta:      Orientation of rune
// angle:      Angle of currently activated rune leaf
// spd:        Rotational angular velocity of rune
// a:          Trigonometric amplitude of rune speed
// w:          Rotation parameter omega of rune
// p:          Angular velocity phase of rune
// r:          Radius of rune

// [ x, y, z, theta, angle, spd ]       [ x, y, z, theta, angle]    
// [ 0, 1, 2,   3,     4,    5  ]       [ 0, 1, 2,   3,     4  ]

// [ x, y, z, theta, angle, p, a, w ]   [ x, y, z, theta, angle]    
// [ 0, 1, 2,   3,     4,   5, 6, 7 ]   [ 0, 1, 2,   3,     4  ]

// [ angle, spd ]   [ angle ]
// [   0  ,  1  ]   [   0   ]

namespace rm {

constexpr double A_MIN = 0.780;
constexpr double A_MAX = 1.045;
constexpr double W_MIN = 1.884;
constexpr double W_MAX = 2.000;
constexpr double B_BASE = 2.090;
constexpr double SMALL_RUNE_SPD = M_PI / 3;
constexpr double R = 0.69852;

struct SmallRuneV2_FuncA {
    template<class T>
    void operator()(const T x0[6], T x1[6]) {
        x1[0] = x0[0];
        x1[1] = x0[1];
        x1[2] = x0[2];
        x1[3] = x0[3];
        x1[4] = x0[4] + dt * x0[5];
        x1[5] = x0[5];
    }
    double dt;
};

struct BigRuneV2_FuncA {
    template<class T>
    void operator()(const T x0[8], T x1[8]) {
        x1[0] = x0[0];
        x1[1] = x0[1];
        x1[2] = x0[2];
        x1[3] = x0[3];
        x1[4] = x0[4]
              + sign * dt * (2.090 - x0[6])
              + sign * x0[6] * ceres::sin(x0[5]) * dt;
        x1[5] = x0[5] + x0[7] * dt;
        x1[6] = x0[6];
        x1[7] = x0[7];
        
    }
    double dt;
    double sign = 0.0;
};

struct SmallRuneV2_FuncH {
    template<typename T>
    void operator()(const T x[6], T y[5]) {
        y[0] = x[0] + R * ceres::cos(x[4]) * ceres::sin(x[3]);
        y[1] = x[1] - R * ceres::cos(x[4]) * ceres::cos(x[3]);
        y[2] = x[2] + R * ceres::sin(x[4]);
        y[3] = x[3];
        y[4] = x[4];
    }
};

struct BigRuneV2_FuncH {
    template<typename T>
    void operator()(const T x[8], T y[5]) {
        y[0] = x[0] + R * ceres::cos(x[4]) * ceres::sin(x[3]);
        y[1] = x[1] - R * ceres::cos(x[4]) * ceres::cos(x[3]);
        y[2] = x[2] + R * ceres::sin(x[4]);
        y[3] = x[3];
        y[4] = x[4];
    }
};

struct RuneV2_SpdFuncA {
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 1) = dt;
    }
};

struct RuneV2_SpdFuncH {
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
    }
};

class RuneV2 {
public:
    RuneV2();
    ~RuneV2() {}

    void push(const Eigen::Matrix<double, 5, 1>& pose, TimePoint t);
    Eigen::Matrix<double, 4, 1> getPose(double append_delay);

    void getStateStr(std::vector<std::string>& str);
    bool getFireFlag(double append_delay);
    void setSmallMatrixQ(double, double, double, double, double, double);
    void setSmallMatrixR(double, double, double, double, double);
    void setBigMatrixQ(double, double, double, double, double, double, double, double);
    void setBigMatrixR(double, double, double, double, double);
    void setSpdMatrixQ(double, double);
    void setSpdMatrixR(double);
    void setRuneType(bool is_big_rune) { is_big_rune_ = is_big_rune; }
    void setAutoFire(double big_spd, double fire_after, double fire_flag_keep, double fire_interval, double to_center) {
        big_rune_fire_spd_ = big_spd;
        fire_after_trans_delay_ = fire_after;
        fire_flag_keep_delay_ = fire_flag_keep;
        fire_interval_delay_ = fire_interval;
        turn_to_center_delay_ = to_center;
    }


private:
    double getAngleTrans(const double, const double);               // Convert angle in model to approach new angle
    bool   getRuneTrans(const double, const double);                // Determine if rune page toggle occurred
    double getSafeSub(const double, const double);                  // Safe subtraction

    int      toggle_ = 0;                                           // Toggle label
    int      update_num_ = 0;                                       // Update count
    bool     is_big_rune_ = false;                                  // Whether it is big rune
    bool     is_rune_trans_ = false;                                // Whether rune toggled
    bool     is_fire_flag_  = false;                                // Whether currently firing

    double   big_rune_fire_spd_      = 1.0;                         // Big rune fire angular velocity
    double   fire_after_trans_delay_ = 0.1;                         // How long to fire after rune toggle
    double   fire_flag_keep_delay_   = 0.1;                         // Fire signal retention time
    double   fire_interval_delay_    = 0.5;                         // Interval between two fires
    double   turn_to_center_delay_   = 1.0;                         // Model retention time
    
    
    EKF<6, 5>          small_model_;                                // Motion model
    EKF<8, 5>          big_model_;                                  // Motion model
    KF<2, 1>           spd_model_;                                  // Angular velocity model

    SmallRuneV2_FuncA  small_funcA_;                                // State transition function of motion model
    BigRuneV2_FuncA    big_funcA_;                                  // State transition function of motion model
    RuneV2_SpdFuncA    spd_funcA_;                                  // State transition function of angular velocity model

    SmallRuneV2_FuncH  small_funcH_;                                // Observation function of motion model
    BigRuneV2_FuncH    big_funcH_;                                  // Observation function of motion model
    RuneV2_SpdFuncH    spd_funcH_;                                  // Observation function of angular velocity model

    TimePoint t_;                                                   // Last update time
    TimePoint t_trans_;                                             // Last rune toggle time
    TimePoint t_fire_;                                              // Last fire time

    SlideAvg<double> center_x_;                                     // Rune center point x coordinate
    SlideAvg<double> center_y_;                                     // Rune center point y coordinate
    SlideAvg<double> center_z_;                                     // Rune center point z coordinate
    SlideAvg<double> theta_;                                        // Rune orientation
    SlideAvg<double> spd_;                                          // Rune speed
};


};

#endif