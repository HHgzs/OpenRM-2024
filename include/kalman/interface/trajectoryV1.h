#ifndef __OPENRM_KALMAN_INTERFACE_TRAJECTORY_V1_H__
#define __OPENRM_KALMAN_INTERFACE_TRAJECTORY_V1_H__
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <algorithm>

// [ x, y, z, vx, vy, vz, ax, ay, az ]  [ x, y, z ]
// [ 0, 1, 2, 3,  4,  5,  6,  7,  8  ]  [ 0, 1, 2 ]

namespace rm {

struct TrajectoryV1_FuncA {
    template<class T>
    void operator()(const T x0[9], T x1[9]) {
        x1[0] = x0[0] + dt * x0[3] + 0.5 * x0[6] * dt * dt;
        x1[1] = x0[1] + dt * x0[4] + 0.5 * x0[7] * dt * dt;
        x1[2] = x0[2] + dt * x0[5] + 0.5 * x0[8] * dt * dt;
        x1[3] = x0[3] + dt * x0[6];
        x1[4] = x0[4] + dt * x0[7];
        x1[5] = x0[5] + dt * x0[8];
        x1[6] = x0[6];
        x1[7] = x0[7];
        x1[8] = x0[8];
    }
    double dt;
};

struct TrajectoryV1_FuncH {
    template<typename T>
    void operator()(const T x[9], T y[3]) {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
    }
};

class TrajectoryV1 {

public:
    TrajectoryV1();
    TrajectoryV1(double keep_delay);
    ~TrajectoryV1() {}

    void push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    Eigen::Matrix<double, 4, 1> getPose(double append_delay);
    double getDistance(double append_delay, double x, double y);

    void setMatrixQ(double, double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double);
    void setKeepDelay(double keep_delay);



private:
    double keep_delay_ = 3.0;

    EKF<9, 3> model_;
    TrajectoryV1_FuncA  funcA_;                                        // 运动模型的状态转移函数
    TrajectoryV1_FuncH  funcH_; 

    TimePoint t_;
};

}

#endif