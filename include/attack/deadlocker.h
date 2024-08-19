#ifndef __OPENRM_ATTACK_DEADLOCKER_H__
#define __OPENRM_ATTACK_DEADLOCKER_H__
#include <attack/attack.h>
#include <vector>

namespace rm {

struct DeadLockerState {
    TimePoint last_t;                       // 目标上一次的时间
    double angle;                           // 目标与图传中心的角度
    bool exist;                             // 目标是否存在
    DeadLockerState() : last_t(getTime()), angle(1e3), exist(false) {}
};

class DeadLocker : public AttackInterface {
public:
    DeadLocker();
    ~DeadLocker() {};

    void push(ArmorID armor_id, double angle, TimePoint t) override;
    ArmorID pop() override;

    void refresh() override;
    void clear() override;

private:
    std::vector<DeadLockerState> state_;

};

}


#endif