#include "solver/polynomial.h"
#include <ceres/ceres.h>

struct Data {
    double x, y, z;
};

// 定义残差函数
struct MultiVarPolynomialResidual {

    MultiVarPolynomialResidual::MultiVarPolynomialResidual(
        const double x, const double y, const double z) : x_(x), y_(y), z_(z) {}
    
    template <typename T>
    bool operator()(const T *const c, T *residual) const {
        int max = x_max;
        int cnt = 1;
        if (y_max > x_max)
            max = y_max;
        // residual[0] = z_ - (c[0] + c[1]*x_ + c[2]*y_ + c[3]*x_*y_ + c[4]*pow(x_, 2) + c[5]*pow(y_, 2));
        residual[0] = z_ - c[0];
        for (int i = 0; i < x_max; i++) {
            for (int j = 0; j < y_max; j++) {
                if (i == 0 && j == 0)
                    continue;

                // std::cout << i << " " << j << std::endl;
                if (i + j < max) {
                    residual[0] -= c[cnt] * pow(x_, i) * pow(y_, j);
                    cnt++;
                    // std::cout << cnt << std::endl;
                }
            }
        }

        return true;
    }
    const double x_, y_, z_;
};

static double R_2 = 0;

static double *fitFactors;

static int fit_x_max, fit_y_max, fit_max;

static int fitFactorsLen = 0;

double rm::getPrediction(double x, double y) {
    double prediction = fitFactors[0];
    int cnt = 1;

    for (int i = 0; i < fit_x_max; i++) {
        for (int j = 0; j < fit_y_max; j++) {
            if (i == 0 && j == 0)
                continue;
            if (i + j < fit_max) {
                prediction += fitFactors[cnt] * pow(x, i) * pow(y, j);
                cnt++;
            }
        }
    }
    return prediction;
}

void rm::initFit(int x_max, int y_max) {

    x_max++;
    y_max++;
    fit_x_max = x_max;
    fit_y_max = y_max;

    fit_max = x_max > y_max ? x_max : y_max;

    if (x_max == 1 && y_max == 1) {
        fitFactorsLen = 0;
        return;
    }

    int cnt = 1;

    for (int i = 0; i < x_max; i++) {
        for (int j = 0; j < y_max; j++) {
            if (i == 0 && j == 0)
                continue;
            if (i + j < fit_max) {
                cnt++;
                // std::cout << cnt << std::endl;
            }
        }
    }
    fitFactors = (double *)malloc(cnt * sizeof(double));

    fitFactorsLen = cnt;
}

void rm::calculateFactors(std::vector<double> &inputs, bool isNeedR_2) {
    // 设置需要优化的参数

    int dataLength = inputs.size();

    ceres::Problem problem;
    for (int i = 0; i < dataLength; ++i) {
        ceres::CostFunction *cost_function =
            new ceres::AutoDiffCostFunction<MultiVarPolynomialResidual, 1, 6>(
                new MultiVarPolynomialResidual(inputs[i * 3], inputs[i * 3 + 1], inputs[i * 3 + 2]));
        problem.AddResidualBlock(cost_function, NULL, fitFactors);
    }

    // 设置求解器选项
    ceres::Solver::Options options;
    options.max_num_iterations = 1000;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;

    // 调用求解器求解优化问题
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    if (!isNeedR_2)
        return;

    double SSE = 0;
    double SST = 0;
    double R_2;
    double d_tmp;
    for (int n = 0; n < dataLength; n++) {
        double x_ = inputs[n * 3];
        double y_ = inputs[n * 3 + 1];
        double z_ = inputs[n * 3 + 2];
        int cnt = 1;

        d_tmp = z_ - fitFactors[0];
        for (int i = 0; i < fit_x_max; i++) {
            for (int j = 0; j < fit_y_max; j++) {
                if (i == 0 && j == 0)
                    continue;
                if (i + j < fit_max) {
                    d_tmp -= fitFactors[cnt] * pow(x_, i) * pow(y_, j);
                    cnt++;
                    // std::cout << cnt << std::endl;
                }
            }
        }

        // std::cout << "残差" << n << " " << d_tmp << std::endl;

        SSE += pow(d_tmp, 2);
        SST += z_ * z_;
    }
    double z_sum = 0;
    for (int i = 0; i < dataLength; i++) {
        z_sum += inputs[i * 3 + 2];
    }
    SST -= z_sum * z_sum / dataLength;

    std::cout << "SSE" << SSE << std::endl;
    std::cout << "SST" << SST << std::endl;
    R_2 = 1 - SSE / SST;
}

void rm::getFitFactors()
{
    int cnt = 1;

    if (!fitFactorsLen)
        return;
    std::cout << fitFactors[cnt] << " + ";

    for (int i = 0; i < fit_x_max; i++) {
        for (int j = 0; j < fit_y_max; j++) {
            if (i == 0 && j == 0)
                continue;
            if (i + j < fit_max) {
                // prediction += fitFactors[cnt] * pow(x, i) * pow(y, j);
                std::cout << fitFactors[cnt] << "x^" << i << "y^" << j << " + ";
                cnt++;
            }
        }
    }
}

double rm::getR_2() {
    return R_2;
}