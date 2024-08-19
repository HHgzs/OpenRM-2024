#ifndef __OPENRM_KALMAN_FILTER_KF_H__
#define __OPENRM_KALMAN_FILTER_KF_H__

#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>

template<int dimX, int dimY>
class KF {
public:
    using MatXX = Eigen::Matrix<double, dimX, dimX>;
    using MatXY = Eigen::Matrix<double, dimX, dimY>;
    using MatYX = Eigen::Matrix<double, dimY, dimX>;
    using MatYY = Eigen::Matrix<double, dimY, dimY>;
    using VecX = Eigen::Matrix<double, dimX, 1>;
    using VecY = Eigen::Matrix<double, dimY, 1>;
    
    KF():
        estimate_X(VecX::Zero()),
        P(MatXX::Identity()),
        Q(MatXX::Identity()),
        R(MatYY::Identity()) {}

    KF(const MatXX& Q0, const MatYY& R0):
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
    MatXX A;
    MatYX H;
    MatXX P;
    MatXX Q;
    MatYY R;
    MatXY K;

    template<class Func>
    VecX predict(Func&& func) {
        
        func(A);
        predict_X = A * estimate_X;
        P = A * P * A.transpose() + Q;
        
        return predict_X;
    }

    template<class Func>
    VecX update(Func&& func, const VecY& Y) {
    
        func(H);
        K = P * H.transpose() * (H * P * H.transpose() + R).inverse();
        estimate_X = predict_X + K * (Y - H * predict_X);
        P = (MatXX::Identity() - K * H) * P;
        
        return estimate_X;
    }
};

#endif
