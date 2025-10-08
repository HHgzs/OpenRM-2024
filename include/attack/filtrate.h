#ifndef __OPENRM_ATTACK_FILTRATE_H__
#define __OPENRM_ATTACK_FILTRATE_H__
#include <attack/attack.h>
#include <vector>

namespace rm {

struct FiltrateState {
    TimePoint last_t;                       // Last time of target
    bool exist;                             // Whether target exists
    FiltrateState() : last_t(getTime()), exist(false) {}
};

class Filtrate : public AttackInterface {
public:
    Filtrate();
    ~Filtrate() {};

    void push(ArmorID armor_id, double angle, TimePoint t) override;
    ArmorID pop() override;

    void refresh() override;
    void clear() override;

private:
    std::vector<FiltrateState> state_;
};

}


#endif