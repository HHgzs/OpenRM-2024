#ifndef __OPENRM_UTILS_PRINT_H__
#define __OPENRM_UTILS_PRINT_H__
#include <iostream>
#include <string>

namespace rm
{
    void print1d(
        double x, std::string s1 = "");
    void print2d(
        double x, double y,
        std::string s1 = "", std::string s2 = "");
    void print3d(
        double x, double y, double z,
        std::string s1 = "", std::string s2 = "", std::string s3 = "");
    void print4d(
        double x, double y, double z, double w,
        std::string s1 = "", std::string s2 = "", std::string s3 = "", std::string s4 = "");
    void print5d(
        double x, double y, double z, double u, double v,
        std::string s1 = "", std::string s2 = "", std::string s3 = "",
        std::string s4 = "", std::string s5 = "");
    void print6d(
        double x, double y, double z,
        double u, double v, double w,
        std::string s1 = "", std::string s2 = "", std::string s3 = "",
        std::string s4 = "", std::string s5 = "", std::string s6 = "");
    void print8d(
        double x, double y, double z,
        double u, double v, double w,
        double a, double b,
        std::string s1 = "", std::string s2 = "", std::string s3 = "",
        std::string s4 = "", std::string s5 = "", std::string s6 = "",
        std::string s7 = "", std::string s8 = "");
    
}

#endif