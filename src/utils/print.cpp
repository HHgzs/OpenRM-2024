#include "utils/print.h"
#include <iostream>
#include <iomanip>
#include <cmath> 
using namespace rm;

void rm::print1d(double x, std::string s1) {
    s1 += ":";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << s1 << std::setw(8) << x << std::endl;
}

void rm::print2d(double x, double y, std::string s1, std::string s2) {
    s1 += ":";
    s2 += ":";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << s1 << std::setw(8) << x << "  " << s2 << std::setw(6) << y << std::endl;
}


void rm::print3d(double x, double y, double z, std::string s1, std::string s2, std::string s3) {
    s1 += ":";
    s2 += ":";
    s3 += ":";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << s1 << std::setw(6) << x << "  " << s2 << std::setw(6) << y << "  " << s3 << std::setw(6) << z << std::endl;

}

void rm::print4d(
    double x, double y, double z, double w,
    std::string s1, std::string s2, std::string s3, std::string s4
) {
    s1 += ":";
    s2 += ":";
    s3 += ":";
    s4 += ":";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << s1 << std::setw(6) << x << "  " << s2 << std::setw(6) << y << "  ";
    std::cout << s3 << std::setw(6) << z << "  " << s4 << std::setw(6) << w << std::endl;
}

void rm::print5d(
    double x, double y, double z, double u, double v,
    std::string s1, std::string s2, std::string s3, std::string s4, std::string s5
) {
    s1 += ":";
    s2 += ":";
    s3 += ":";
    s4 += ":";
    s5 += ":";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << s1 << std::setw(6) << x << "  " << s2 << std::setw(6) << y << "  " << s3 << std::setw(6) << z << "  ";
    std::cout << s4 << std::setw(6) << u << "  " << s5 << std::setw(6) << v << std::endl;
}

void rm::print6d(
    double x, double y, double z,
    double u, double v, double w,
    std::string s1, std::string s2, std::string s3,
    std::string s4, std::string s5, std::string s6
) {
    s1 += ":";
    s2 += ":";
    s3 += ":";
    s4 += ":";
    s5 += ":";
    s6 += ":";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << s1 << std::setw(6) << x << "  " << s2 << std::setw(6) << y << "  " << s3 << std::setw(6) << z << "  ";
    std::cout << s4 << std::setw(6) << u << "  " << s5 << std::setw(6) << v << "  " << s6 << std::setw(6) << w << std::endl;
}

void rm::print8d(
    double x, double y, double z,
    double u, double v, double w,
    double a, double b,
    std::string s1, std::string s2, std::string s3, std::string s4,
    std::string s5, std::string s6, std::string s7, std::string s8
) {
    s1 += ":";
    s2 += ":";
    s3 += ":";
    s4 += ":";
    s5 += ":";
    s6 += ":";
    s7 += ":";
    s8 += ":";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << s1 << std::setw(6) << x << "  " << s2 << std::setw(6) << y << "  " << s3 << std::setw(6) << z << "  ";
    std::cout << s4 << std::setw(6) << u << "  " << s5 << std::setw(6) << v << "  " << s6 << std::setw(6) << w << "  ";
    std::cout << s7 << std::setw(6) << a << "  " << s8 << std::setw(6) << b << std::endl;
}