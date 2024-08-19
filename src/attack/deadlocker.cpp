#include "attack/deadlocker.h"
using namespace rm;

DeadLocker::DeadLocker() : AttackInterface() {
    state_.resize(ArmorID::ARMOR_ID_COUNT);
    this->clear();
}

void DeadLocker::push(ArmorID armor_id, double angle, TimePoint t)  {
    if (armor_id == ARMOR_ID_UNKNOWN) return;
    state_[armor_id].angle = angle;
    state_[armor_id].last_t = t;
    state_[armor_id].exist = true;
}

ArmorID DeadLocker::pop() {
    TimePoint now_t = getTime();
    for(auto& focus : state_) {
        if(!focus.exist) continue;
        double dt = getDoubleOfS(focus.last_t, now_t);
        if(dt > exist_dt_) {
            focus.exist = false;
        }
    }

    if(focus_id_ != ARMOR_ID_UNKNOWN && state_[focus_id_].exist && (state_[focus_id_].angle < M_PI)) {
        return focus_id_;
    } else {
        focus_id_ = ARMOR_ID_UNKNOWN;
        for(size_t i = 0; i < state_.size(); i++) {
            if(state_[i].exist && (state_[i].angle < state_[focus_id_].angle)) {
                focus_id_ = (ArmorID)i;
            }
        }
        return focus_id_;
    }
}

void DeadLocker::refresh() {
    focus_id_ = ARMOR_ID_UNKNOWN;
    TimePoint now_t = getTime();
    for(auto& focus : state_) {
        double dt = getDoubleOfS(focus.last_t, now_t);
        if(dt > exist_dt_) {
            focus.exist = false;
        }
    }
}

void DeadLocker::clear() {
    for(auto& focus : state_) {
        focus.exist = false;
        focus.angle = 1e3;
        focus.last_t = getTime();
    }
    focus_id_ = ARMOR_ID_UNKNOWN;
}