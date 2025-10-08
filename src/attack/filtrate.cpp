#include "attack/filtrate.h"
using namespace rm;

Filtrate::Filtrate() : AttackInterface() {
    state_.resize(ArmorID::ARMOR_ID_COUNT);
    focus_id_ = ARMOR_ID_UNKNOWN;
}

void Filtrate::push(ArmorID armor_id, double angle, TimePoint t)  {
    if (!isValidArmorID(armor_id, valid_byte_)) return;
    state_[armor_id].last_t = t;
    state_[armor_id].exist = true;
}

ArmorID Filtrate::pop() {
    TimePoint now_t = getTime();
    for(size_t i = 0; i < state_.size(); i++) {
        if (!isValidArmorID((ArmorID)i, valid_byte_)) {
            state_[i].exist = false;
            continue;
        }

        double dt = getDoubleOfS(state_[i].last_t, now_t);
        if (getDoubleOfS(state_[i].last_t, now_t) > exist_dt_) {
            state_[i].exist = false;
            continue;
        }
    }

    if (state_[focus_id_].exist && isValidArmorID(focus_id_, valid_byte_)) {
        return focus_id_;
    }

    double min_dt = -1;
    focus_id_ = ARMOR_ID_UNKNOWN;
    for(size_t i = 0; i < state_.size(); i++) {
        if (!state_[i].exist || !isValidArmorID((ArmorID)i, valid_byte_)) {
            continue;
        }

        double dt = getDoubleOfS(state_[i].last_t, now_t);
        if (dt < min_dt || min_dt < 0) {
            focus_id_ = (ArmorID)i;
            min_dt = dt;
        }
    }
    return focus_id_;
}

void Filtrate::refresh() {
    focus_id_ = ARMOR_ID_UNKNOWN;
    TimePoint now_t = getTime();
    for (auto& focus : state_) {
        if (getDoubleOfS(focus.last_t, now_t) > exist_dt_) {
            focus.exist = false;
        }
    }
}

void Filtrate::clear() {
    for (auto& focus : state_) {
        focus.exist = false;
        focus.last_t = getTime();
    }
    focus_id_ = ARMOR_ID_UNKNOWN;
}