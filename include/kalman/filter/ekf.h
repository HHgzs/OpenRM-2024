#ifndef __OPENRM_KALMAN_FILTER_EKF_H__
#define __OPENRM_KALMAN_FILTER_EKF_H__

#include <Eigen/Dense>
#include <ceres/jet.h>
#include <opencv2/core/eigen.hpp>


template<int dimX, int dimY>
class EKF {
public:
    using MatXX = Eigen::Matrix<double, dimX, dimX>;
    using MatXY = Eigen::Matrix<double, dimX, dimY>;
    using MatYX = Eigen::Matrix<double, dimY, dimX>;
    using MatYY = Eigen::Matrix<double, dimY, dimY>;
    using VecX = Eigen::Matrix<double, dimX, 1>;
    using VecY = Eigen::Matrix<double, dimY, 1>;

    EKF():
        estimate_X(VecX::Zero()),
        P(MatXX::Identity()),
        Q(MatXX::Identity()),
        R(MatYY::Identity()) {}

    EKF(const MatXX& Q0, const MatYY& R0):
        estimate_X(VecX::Zero()),
        P(MatXX::Identity()),
        Q(Q0),
        R(R0) {}

    void restart() {
        estimate_X = VecX::Zero();
        P = MatXX::Identity();
    }

    VecX estimate_X;
    VecX predict_X;
    VecY predict_Y;
    MatXX jacobi_F;
    MatYX jacobi_H;
    MatXX P;
    MatXX Q;
    MatYY R;
    MatXY K;

    template<class Func>
    VecX predict(Func&& func) {
        ceres::Jet<double, dimX> estimate_jet_X[dimX];

        for (int i = 0; i < dimX; i++) {
            estimate_jet_X[i].a = estimate_X[i];
            estimate_jet_X[i].v[i] = 1;
        }

        ceres::Jet<double, dimX> predict_jet_X[dimX];

        func(estimate_jet_X, predict_jet_X);

        for (int i = 0; i < dimX; i++) {
            predict_X[i] = predict_jet_X[i].a;
            jacobi_F.block(i, 0, 1, dimX) = predict_jet_X[i].v.transpose();
        }

        P = jacobi_F * P * jacobi_F.transpose() + Q;

        return predict_X;
    }

    template<class Func>
    VecX update(Func&& func, const VecY& Y) {
        ceres::Jet<double, dimX> predict_jet_X[dimX];

        for (int i = 0; i < dimX; i++) {
            predict_jet_X[i].a = predict_X[i];
            predict_jet_X[i].v[i] = 1;
        }

        ceres::Jet<double, dimX> predict_jet_Y[dimY];

        func(predict_jet_X, predict_jet_Y);

        for (int i = 0; i < dimY; i++) {
            predict_Y[i] = predict_jet_Y[i].a;
            jacobi_H.block(i, 0, 1, dimX) = predict_jet_Y[i].v.transpose();
        }

        K = P * jacobi_H.transpose() * (jacobi_H * P * jacobi_H.transpose() + R).inverse();
        estimate_X = predict_X + K * (Y - predict_Y);
        P = (MatXX::Identity() - K * jacobi_H) * P;

        return estimate_X;
    }
};

#endif