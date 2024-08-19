#include "kalman/interface/trajectoryV1.h"
#include "utils/print.h"
#include "uniterm/uniterm.h"
#include <cmath>
using namespace std;
using namespace rm;

// [ x, y, z, vx, vy, vz, ax, ay, az ]  [ x, y, z ]
// [ 0, 1, 2, 3,  4,  5,  6,  7,  8  ]  [ 0, 1, 2 ]

TrajectoryV1::TrajectoryV1() {
    t_ = getTime();
    setMatrixQ(1.0, 1.0, 1.0, 10.0, 10.0, 10.0, 100.0, 100.0, 100.0);
    setMatrixR(0.001, 0.001, 0.001);
}

TrajectoryV1::TrajectoryV1(double keep_delay) : model_(), keep_delay_(keep_delay) {
    t_ = getTime();
    setMatrixQ(1.0, 1.0, 1.0, 10.0, 10.0, 10.0, 100.0, 100.0, 100.0);
    setMatrixR(0.001, 0.001, 0.001);
}

void TrajectoryV1::push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
    double dt = getDoubleOfS(t_, t);
    t_ = t;

    Eigen::Matrix<double, 3, 1> pose_3d;
    pose_3d << pose[0], pose[1], pose[2];

    funcA_.dt = dt;
    model_.predict(funcA_);
    model_.update(funcH_, pose_3d); 
}
    
Eigen::Matrix<double, 4, 1> TrajectoryV1::getPose(double append_delay) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);

    if (sys_delay > keep_delay_) {
        return Eigen::Matrix<double, 4, 1>::Zero();
    }
    double dt = sys_delay + append_delay;

    Eigen::Matrix<double, 4, 1> pose;
    pose << model_.estimate_X[0] + model_.estimate_X[3] * dt,
            model_.estimate_X[1] + model_.estimate_X[4] * dt,
            model_.estimate_X[2] + model_.estimate_X[5] * dt,
            1.0;
    return pose;
}

double TrajectoryV1::getDistance(double append_delay, double x, double y) {
    auto now = getTime();
    double sys_delay = getDoubleOfS(t_, now);

    if (sys_delay > keep_delay_) {
        return -1.0;
    }
    double dt = sys_delay + append_delay;

    double x_center = model_.estimate_X[0] + model_.estimate_X[3] * dt;
    double y_center = model_.estimate_X[1] + model_.estimate_X[4] * dt;
    return sqrt((x - x_center) * (x - x_center) + (y - y_center) * (y - y_center));
}

void TrajectoryV1::setMatrixQ(double q0, double q1, double q2, double q3, double q4, double q5, double q6, double q7, double q8) {
    model_.Q << q0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, q1, 0, 0, 0, 0, 0, 0, 0,
                0, 0, q2, 0, 0, 0, 0, 0, 0,
                0, 0, 0, q3, 0, 0, 0, 0, 0,
                0, 0, 0, 0, q4, 0, 0, 0, 0,
                0, 0, 0, 0, 0, q5, 0, 0, 0,
                0, 0, 0, 0, 0, 0, q6, 0, 0,
                0, 0, 0, 0, 0, 0, 0, q7, 0,
                0, 0, 0, 0, 0, 0, 0, 0, q8;
}

void TrajectoryV1::setMatrixR(double r0, double r1, double r2) {
    model_.R << r0, 0, 0,
                0, r1, 0,
                0, 0, r2;
}

void TrajectoryV1::setKeepDelay(double keep_delay) {
    keep_delay_ = keep_delay;
}
