#ifndef __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V4_H__
#define __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V4_H__
#include <memory>
#include <vector>
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <structure/slidestd.hpp>

// [ x, y, z, v, vz, angle, w, a ]  [ x, y, z ]
// [ 0, 1, 2, 3, 4,    5,   6, 7 ]  [ 0, 1, 2 ]

namespace rm {

struct TrackQueueV4_FuncA {
    template<class T>
    void operator()(const T x0[8], T x1[8]) {
        x1[0] = x0[0] + dt * x0[3] * ceres::cos(x0[5]) + 0.5 * dt * dt * x0[7] * ceres::cos(x0[5]);
        x1[1] = x0[1] + dt * x0[3] * ceres::sin(x0[5]) + 0.5 * dt * dt * x0[7] * ceres::sin(x0[5]);
        x1[2] = x0[2] + dt * x0[4];
        x1[3] = x0[3] + dt * x0[7];
        x1[4] = x0[4];
        x1[5] = x0[5] + dt * x0[6];
        x1[6] = x0[6];
        x1[7] = x0[7];
    }
    double dt;
};

struct TrackQueueV4_FuncH {
    template<typename T>
    void operator()(const T x[8], T y[3]) {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
    }
};

class TQstateV4 {
public:
    TimePoint last_t;                       // Last time of target
    Eigen::Matrix<double, 4, 1> last_pose;  // Last position of target
    EKF<8, 3> *model;                      // Target motion model
    int count;                              // Update count of this target
    int keep;                               // Keep count of this target
    bool available;                         // Whether this target is available

    TQstateV4() : count(0), keep(5), available(false) {
        last_t = getTime();
        model = new EKF<8, 3>();
    }

    void refresh(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
        this->last_t = t;
        this->last_pose = pose;
        this->count += 1;
        this->keep = 5;
        this->available = true;
    }
};

class TrackQueueV4 {
public:
    TrackQueueV4() {}
    TrackQueueV4(int count, double distance, double delay);
    ~TrackQueueV4() {}

    void push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t);                       // Push single target information
    void update();                                                                   // Update once per frame

public:
    void setCount(int c) { this->count_ = c; }                                       // Set minimum update count for model availability
    void setDistance(double d) { this->distance_ = d; }                              // Set maximum movement distance to be considered same target
    void setDelay(double d) { this->delay_ = d; }                                    // Set maximum delay without model reset
    void setMatrixQ(double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double);

    Eigen::Matrix<double, 4, 1> getPose(double append_delay);                        // Get pose predicted by model
    bool getPose(Eigen::Matrix<double, 4, 1>& pose, TimePoint& t);                    // Get
    
    void getStateStr(std::vector<std::string>& str);                                 // Get target state information string
    bool getFireFlag();                                                              // Determine if fire condition is met


private:
    double getDistance(
        const Eigen::Matrix<double, 4, 1>& this_pose,
        const Eigen::Matrix<double, 4, 1>& last_pose);                               // Distance between two targets
    double getDistance(
        const Eigen::Matrix<double, 3, 1>& this_pose,
        const Eigen::Matrix<double, 3, 1>& last_pose);                               // Distance between two targets
    
private:
    int    count_        = 10;              // Minimum update count to maintain stable state
    double distance_     = 0.15;            // Maximum movement distance to be considered same target
    double delay_        = 0.5;             // Maximum update delay to be considered same target

    TQstateV4* last_state_   = nullptr;     // Last state

    TrackQueueV4_FuncA funcA_;              // State transition function of motion model
    TrackQueueV4_FuncH funcH_;              // Observation function of motion model

    Eigen::Matrix<double, 8, 8> matrixQ_; // Process noise covariance matrix of motion model
    Eigen::Matrix<double, 3, 3> matrixR_;   // Observation noise covariance matrix of motion model

public:
    std::vector<TQstateV4*> list_;          // Target state list
};

}

#endif