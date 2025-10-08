#ifndef __OPENRM_STRUCTURE_VECTOR6D_HPP__
#define __OPENRM_STRUCTURE_VECTOR6D_HPP__

namespace rm {
class Vector6d {
public:
    Vector6d() {}
    Vector6d(double x, double y, double z) : x(x), y(y), z(z) {}
    Vector6d(double x, double y, double z, double yaw) : x(x), y(y), z(z), yaw(yaw) {}
    Vector6d(double x, double y, double z, double yaw, double pitch, double roll)
        : x(x), y(y), z(z), yaw(yaw), pitch(pitch), roll(roll) {}
    ~Vector6d() {}

    double x = 0;
    double y = 0;
    double z = 0;

    double yaw = 0;
    double pitch = 0;
    double roll = 0;

    // 加法运算符重载


    Vector6d operator+(double d) const {
        return Vector6d(x + d, y + d, z + d, yaw + d, pitch + d, roll + d);
    }
    Vector6d operator-(double d) const {
        return Vector6d(x - d, y - d, z - d, yaw - d, pitch - d, roll - d);
    }
    Vector6d operator*(double d) const {
        return Vector6d(x * d, y * d, z * d, yaw * d, pitch * d, roll * d);
    }
    Vector6d operator/(double d) const {
        if (d != 0.0) return Vector6d(x / d, y / d, z / d, yaw / d, pitch / d, roll / d);
        else return *this;
    }


    Vector6d operator+(const Vector6d& v) const {
        return Vector6d(x + v.x, y + v.y, z + v.z, yaw + v.yaw, pitch + v.pitch, roll + v.roll);
    }
    Vector6d operator-(const Vector6d& v) const {
        return Vector6d(x - v.x, y - v.y, z - v.z, yaw - v.yaw, pitch - v.pitch, roll - v.roll);
    }
    Vector6d operator*(const Vector6d& v) const {
        return Vector6d(x * v.x, y * v.y, z * v.z, yaw * v.yaw, pitch * v.pitch, roll * v.roll);
    }
    Vector6d operator/(const Vector6d& v) const {
        if (v.x != 0 && v.y != 0 && v.z != 0 && v.yaw != 0 && v.pitch != 0 && v.roll != 0) {
            return Vector6d(x / v.x, y / v.y, z / v.z, yaw / v.yaw, pitch / v.pitch, roll / v.roll);
        } else {
            return *this;
        }
    }

    // 赋值运算符重载
    Vector6d& operator=(const Vector6d& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        yaw = v.yaw;
        pitch = v.pitch;
        roll = v.roll;
        return *this;
    }

    Vector6d& operator+=(const Vector6d& v) {
        return *this = *this + v;
    }
    Vector6d& operator-=(const Vector6d& v) {
       return *this = *this - v;
    }
    Vector6d& operator*=(const Vector6d& v) {
        return *this = *this * v;
    }
    Vector6d& operator/=(const Vector6d& v) {
        return *this = *this / v;
    }


};
}


#endif