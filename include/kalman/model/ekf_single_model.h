#ifndef __OPENRM_KALMAN_MODEL_EKF_SINGLE_MODEL_H__
#define __OPENRM_KALMAN_MODEL_EKF_SINGLE_MODEL_H__

// [ x, y, z, theta, vx, vy, vz, ax, ay]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5,  6,  7,  8 ]  [ 0, 1, 2,   3  ]

namespace EKF_SM {

const int dimX = 9;
const int dimY = 4;

struct FuncA {
    template<class T>
    void operator()(const T x0[dimX], T x1[dimX]) {
        x1[0] = x0[0] + dt * x0[4] + 0.5 * x0[7] * dt * dt;
        x1[1] = x0[1] + dt * x0[5] + 0.5 * x0[8] * dt * dt;
        x1[2] = x0[2] + dt * x0[6];
        x1[3] = x0[3];
        x1[4] = x0[4] + x0[7] * dt;
        x1[5] = x0[5] + x0[8] * dt;
        x1[6] = x0[6];
        x1[7] = x0[7];
        x1[8] = x0[8];
    }
    double dt;

};

struct FuncH {
    template<typename T>
    void operator()(const T x[dimX], T y[dimY]) {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
        y[3] = x[3];
    }
};
}





#endif