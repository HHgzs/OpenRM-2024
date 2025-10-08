#ifndef __OPENRM_ATTACK_DEADLOCKER_H__
#define __OPENRM_ATTACK_DEADLOCKER_H__
#include <attack/attack.h>
#include <vector>

namespace rm {

struct DeadLockerState {
    TimePoint last_t;                       // Last time of target
    double angle;                           // Angle between target and image transmission center
    bool exist;                             // Whether target exists
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