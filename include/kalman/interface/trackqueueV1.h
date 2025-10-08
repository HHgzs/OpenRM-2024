#ifndef __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V1_H__
#define __OPENRM_KALMAN_INTERFACE_TRACK_QUEUE_V1_H__
#include <memory>
#include <vector>
#include <utils/timer.h>
#include <kalman/filter/kf.h>
#include <structure/slidestd.hpp>

// [ x, y, z, theta, vx, vy]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5 ]  [ 0, 1, 2,   3  ]

namespace rm {

struct TrackQueueV1_FuncA{
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 4) = dt;
        A(1, 5) = dt;
    }
};

struct TrackQueueV1_FuncH{
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
        H(1, 1) = 1;
        H(2, 2) = 1;
        H(3, 3) = 1;
    }
};

class TQstateV1 {
public:
    TimePoint last_t;                       // Last time of target
    Eigen::Matrix<double, 4, 1> last_pose;  // Last position of target
    KF<6, 4> *model;                        // Target motion model
    SlideStd<double> *v_std;                // Velocity standard deviation of target motion model
    int count;                              // Update count of this target
    bool exist;                             // Whether target exists
    bool available;                         // Target information available

    TQstateV1() :
        last_t(getTime()),
        count(0),
        exist(false),
        available(false) {
        model = new KF<6, 4>();
        v_std = new SlideStd<double>(10);
    }

    void clear() {
        this->count = 0;
        this->exist = false;
        this->available = false;
        this->model->restart();
        this->v_std->clear();
    }

    void update(const Eigen::Matrix<double, 4, 1>& pose, TimePoint t) {
        this->last_t = t;
        this->last_pose = pose;
        this->count += 2;
        this->exist = true;
        this->available = true;
    }
};

class TrackQueueV1 {
public:
    TrackQueueV1();
    TrackQueueV1(int, double, double, double, double);
    ~TrackQueueV1() {}

    void push(Eigen::Matrix<double, 4, 1>& pose, TimePoint t);
    void update();


public:
    void setMinCount(int c) { this->min_count_ = c; }
    void setMaxDistance(double d) { this->max_distance_ = d; }
    void setMaxDelay(double d) { this->max_delay_ = d; }
    void setToggleAngleOffset(double d) { this->toggle_angle_offset_ = d; }
    void setMaxStd(double d) { this->max_std_ = d; }
    void setMatrixQ(double, double, double, double, double, double);
    void setMatrixR(double, double, double, double);
    int getToggle() { return last_toggle_; }
    KF<6, 4>* getModel();                                                            // Get updated model                                 
    TimePoint getLastTime();                                                         // Get last update time
    double getStd();                                                                 // Get velocity standard deviation of last updated target
    bool isStdValid();                                                               // Determine if available based on velocity standard deviation
    Eigen::Matrix<double, 4, 1> getPose();                                           // Directly get last updated pose
    Eigen::Matrix<double, 4, 1> getPose(double delay);                               // Get pose predicted by model

private:
    double getAngleOffset(const Eigen::Matrix<double, 4, 1>& pose);                  // Angle between target direction and its connection line with vehicle
    int getMinAngleOffset();                                                         // Get target index with minimum angle
    double getDistance(const Eigen::Matrix<double, 4, 1>& this_pose, const Eigen::Matrix<double, 4, 1>& last_pose);       // Distance between two targets
    bool isDistanceValid(double d);                                                  // Determine if can be considered same target based on movement distance 
    bool isDelayValid(const TimePoint& this_t, const TimePoint& last_t);             // Determine if should be discarded based on update delay
    bool isCountKeepValid(int count);                                                // Determine if can maintain state based on update count
    bool isCountUseValid(int count);                                                 // Determine if available based on update count
    bool isAngleOffsetValid(double angle0, double angle1);                           // Determine if can toggle based on angle difference
    

private:
    int min_count_ = 5;                     // Minimum update count to maintain stable state
    double max_distance_ = 0.1;             // Maximum movement distance to be considered same target
    double max_delay_ = 0.3;                // Maximum update delay to be considered same target
    double toggle_angle_offset_ = 0.17;     // Angle difference condition to trigger toggle
    double max_std_ = 0.1;                  // Maximum value of velocity standard deviation

    int last_index_ = -1;                   // Output target index of last update
    int last_toggle_ = 0;                   // Toggle label of last update

    TrackQueueV1_FuncA funcA_;              // State transition function of motion model
    TrackQueueV1_FuncH funcH_;              // Observation function of motion model
    Eigen::Matrix<double, 6, 6> matrixQ_;   // Process noise covariance matrix of motion model
    Eigen::Matrix<double, 4, 4> matrixR_;   // Observation noise covariance matrix of motion model

public:
    std::vector<TQstateV1> list_;            // Target state list
};

}

#endif