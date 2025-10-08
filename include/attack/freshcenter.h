#ifndef __OPENRM_ATTACK_FRESHCENTER_H__
#define __OPENRM_ATTACK_FRESHCENTER_H__
#include <attack/attack.h>
#include <vector>

namespace rm {

struct FreshCenterState {
    TimePoint last_t;                       // Last time of target
    double angle;                           // Angle between target and image transmission center
    bool exist;                             // Whether target exists
    FreshCenterState() : last_t(getTime()), angle(1e3), exist(false) {}
};

class FreshCenter : public AttackInterface {
public:
    FreshCenter();
    ~FreshCenter() {};

    void push(ArmorID armor_id, double angle, TimePoint t) override;
    ArmorID pop() override;

    void refresh() override;
    void clear() override;

private:
    std::vector<FreshCenterState> state_;

};

}


#endif