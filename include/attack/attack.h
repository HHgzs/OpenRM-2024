#ifndef __OPENRM_ATTACK_ATTACK_H__
#define __OPENRM_ATTACK_ATTACK_H__
#include <utils/timer.h>
#include <structure/stamp.hpp>
#include <structure/enums.hpp>

namespace rm {

enum ValidByteMask {
    VALID_BYTE_MASK_SENTRY    = 0x01,    // 0000 0001  0x01  哨兵
    VALID_BYTE_MASK_HERO      = 0x02,    // 0000 0010  0x02  英雄
    VALID_BYTE_MASK_ENGINEER  = 0x04,    // 0000 0100  0x04  工程
    VALID_BYTE_MASK_INFANTRY3 = 0x08,    // 0000 1000  0x08  步兵3
    VALID_BYTE_MASK_INFANTRY4 = 0x10,    // 0001 0000  0x10  步兵4
    VALID_BYTE_MASK_INFANTRY5 = 0x20,    // 0010 0000  0x20  步兵5
    VALID_BYTE_MASK_TOWER     = 0x40,    // 0100 0000  0x40  前哨站 基地
    VALID_BYTE_MASK_ALL       = 0x80     // 1000 0000  0x80  全打
};

bool isValidArmorID(ArmorID armor_id, char valid_byte);

double getAngleOffsetTargetToReferee(
    const double yaw,
    const double pitch,
    const double target_x,
    const double target_y,
    const double target_z,
    const double referee_x,
    const double referee_y,
    const double referee_z,
    const double referee_yaw = 0.0,
    const double referee_pitch = 0.0,
    const double axis_x = 0.0,
    const double axis_y = 0.0,
    const double axis_z = 0.0);



class AttackInterface {
public:
    AttackInterface() {};
    ~AttackInterface() {};

    virtual void push(ArmorID armor_id, double angle, TimePoint t) = 0;
    virtual ArmorID pop() = 0;
    virtual void refresh() = 0;
    virtual void clear() = 0;

    void setFocusID(ArmorID armor_id) { focus_id_ = armor_id; };
    void setExistDt(double dt) { exist_dt_ = dt; }
    void setValidID(char valid_byte) { valid_byte_ = valid_byte; };


protected:
    ArmorID focus_id_;
    double exist_dt_ = 1.0;
    char valid_byte_ = 0xFF;

};


class AttackDisplayer {

public:
    AttackDisplayer(int canvas_side = 500, double max_dist = 5) : canvas_side_(canvas_side), max_dist_(max_dist) {
        canvas_ = cv::Mat(canvas_side_, canvas_side_, CV_8UC3, cv::Scalar(50, 50, 50));
    }
    ~AttackDisplayer() {};

    void push(int id, int color, double x, double y);
    void push(int id, double x, double y);
    void push(double body_angle, double head_angle);
    cv::Mat refresh();


private:
    int canvas_side_ = 500;
    double max_dist_ = 5;
    cv::Mat canvas_;

};

}

#endif