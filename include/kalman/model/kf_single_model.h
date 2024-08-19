#ifndef __OPENRM_KALMAN_MODEL_KF_SINGLE_MODEL_H__
#define __OPENRM_KALMAN_MODEL_KF_SINGLE_MODEL_H__

// [ x, y, z, theta, vx, vy]  [ x, y, z, theta]
// [ 0, 1, 2,   3,   4,  5 ]  [ 0, 1, 2,   3  ]

namespace KF_SM {

const int dimX = 6;
const int dimY = 4;

struct FuncA{
    double dt;
    template<class T>
    void operator()(T& A) {
        A = T::Identity();
        A(0, 4) = dt;
        A(1, 5) = dt;
    }
};

struct FuncH{
    template<class T>
    void operator()(T& H) {
        H = T::Zero();
        H(0, 0) = 1;
        H(1, 1) = 1;
        H(2, 2) = 1;
        H(3, 3) = 1;
    }
};

}


#endif 




