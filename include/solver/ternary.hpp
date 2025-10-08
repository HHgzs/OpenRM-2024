#ifndef __OPENRM_SOLVER_TRICHOTOMY_H__
#define __OPENRM_SOLVER_TRICHOTOMY_H__

namespace rm {

template<typename T>
double ternarySearch(double left, double right, const T& func, double epsilon) {
    while (right - left > epsilon) {
        double mid1 = left + (right - left) / 3;
        double mid2 = right - (right - left) / 3;

        double f1 = func(mid1);
        double f2 = func(mid2);

        if (f1 < f2) {
            right = mid2;
        } else {
            left = mid1;
        }
    }
    return (left + right) / 2;
}

}
#endif