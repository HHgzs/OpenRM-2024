#ifndef __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V2_H__
#define __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V2_H__
#include <memory>
#include <vector>
#include <utils/timer.h>
#include <kalman/filter/ekf.h>
#include <structure/slidestd.hpp>

// [ x, y, z, theta, vx, vy, vz, omega, ax, ay, b  ]  [ x, y, z, theta ]
// [ 0, 1, 2,   3,   4,  5,  6,    7,   8,  9,  10 ]  [ 0, 1, 2,   3   ]

namespace rm {

struct TrackQueueV2_FuncA {
    template<class T>
    void operator()(const T x0[11], T x1[11]) {
        x1[0] = x0[0] + dt * x0[4] + 0.5 * x0[8] * dt * dt;
        x1[1] = x0[1] + dt * x0[5] + 0.5 * x0[9] * dt * dt;
        x1[2] = x0[2] + dt * x0[6];
        x1[3] = x0[3] + dt * x0[7] + 0.5 * x0[10] * dt * dt;
        x1[4] = x0[4] + dt * x0[8];
        x1[5] = x0[5] + dt * x0[9];
        x1[6] = x0[6];
        x1[7] = x0[7] + dt * x0[10];
        x1[8] = x0[8];
        x1[9] = x0[9];
    }
    double dt;
};

struct TrackQueueV2_FuncH {
    template<typename T>
    void operator()(const T x[11], T y[4]) {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
        y[3] = x[3];
    }
};

class TQstateV2 {
public:
    TimePoint last_t;                       // Last time of target
    Eigen::Matrix<double, 4, 1> last_pose;  // Last position of target
    EKF<11, 4> *model;                      // Target motion model
    SlideStd<double> *v_std;                // Velocity standard deviation of target motion model
    SlideStd<double> *a_std;                // Acceleration standard deviation of target motion model
    SlideStd<double> *w_std;                // Angular velocity standard deviation of target motion model
    int count;                              // Update count of this target
    int keep;                               // Keep count of this target
    bool exist;                             // Whether target exists
    bool available;                         // Target information available

    TQstateV2() :
        last_t(getTime()),
        count(0),
        keep(5),
        exist(false),
        available(false) {
        model = new EKF<11, 4>();
        v_std = new SlideStd<double>(5);
        a_std = new SlideStd<double>(5);
        w_std = new SlideStd<double>(5);
    }

    void clear() {
        this->count = 0;
        this->keep = 5;
        this->exist = false;
        this->available = false;
        this->model->restart();
        this->v_std->clear();
        this->a_std->clear();
        this->w_std->clear();
    }

    void update(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
        this->last_t = t;
        this->last_pose = pose;
        this->count += 1;
        this->keep = 5;
        this->exist = true;
        this->available = true;
    }
};

class TrackQueueV2 {
public:
    TrackQueueV2() {}
    TrackQueueV2(int count, double distance, double delay, double angle_diff, double toggle_angle);
    ~TrackQueueV2() {}

    void push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t);                       // Push single target information
    void update();                                                                   // Update once per frame

public:
    void setCount(int c) { this->count_ = c; }                                       // Set minimum update count for model availability
    void setDistance(double d) { this->distance_ = d; }                              // Set maximum movement distance to be considered same target
    void setDelay(double d) { this->delay_ = d; }                                    // Set maximum delay without model reset
    void setAngleDiffer(double d) { this->angle_diff_ = d; }                         // Set maximum angle for same target
    void setToggleAngle(double d) { this->toggle_angle_ = d; }                       // Set angle difference between toggle target and center line
    void setFireValue(double sv, double sw, double sa, double angle) {
        this->fire_std_v_ = sv;
        this->fire_std_w_ = sw;
        this->fire_std_a_ = sa;
        this->fire_angle_ = angle;
    }

    void setMatrixQ(double, double, double, double, double, double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);

    int         getToggle() { return last_toggle_; }                                 // Get toggle label
    EKF<11, 4>* getModel();                                                          // Get updated model                                 
    TimePoint   getLastTime();                                                       // Get last update time
    void        getStateStr(std::vector<std::string>& str);                          // Get target state information string                                                            

    Eigen::Matrix<double, 4, 1> getPose();                                           // Directly get last passed pose
    Eigen::Matrix<double, 4, 1> getPose(double delay);                               // Get pose predicted by model

    bool isStdStable();                                                              // Determine if parameter fitting standard deviation is stable
    bool isFireValid(const Eigen::Matrix<double, 4, 1>& pose);                       // Determine if fire condition is met



private:
    double getDistance(
        const Eigen::Matrix<double, 4, 1>& this_pose,
        const Eigen::Matrix<double, 4, 1>& last_pose);                               // Distance between two targets
    double getAngleDiff(double angle0, double angle1);                               // Absolute value of difference between any two angles
    double getAngleOffset(const Eigen::Matrix<double, 4, 1>& pose);                  // Angle between target direction and vehicle connection line
    int    getMinAngleOffset();                                                      // Get target index with minimum angle
    
    
private:
    int    count_        = 10;              // Minimum update count to maintain stable state
    double distance_     = 0.1;             // Maximum movement distance to be considered same target
    double delay_        = 0.3;             // Maximum update delay to be considered same target
    double angle_diff_   = 0.5;             // Maximum angle difference to be considered same plate
    double toggle_angle_ = 0.17;            // Angle difference condition to trigger toggle

    int    last_index_   = -1;              // Output target index of last update
    int    last_toggle_  = 0;               // Toggle label of last update

    double fire_std_v_   = 0.1;             // Fire velocity standard deviation
    double fire_std_w_   = 0.1;             // Fire angular velocity standard deviation
    double fire_std_a_   = 0.1;             // Fire acceleration standard deviation
    double fire_angle_   = 0.5;             // Fire angle

    TrackQueueV2_FuncA funcA_;              // State transition function of motion model
    TrackQueueV2_FuncH funcH_;              // Observation function of motion model

    Eigen::Matrix<double, 11, 11> matrixQ_; // Process noise covariance matrix of motion model
    Eigen::Matrix<double, 4, 4> matrixR_;   // Observation noise covariance matrix of motion model

public:
    std::vector<TQstateV2> list_;          // Target state list
};

}

#endif