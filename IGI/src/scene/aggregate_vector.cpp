#include "igiscene/aggregate_vector.h"

bool igi::aggregate_vector::isHit(const ray &r) const {
    for (auto &e : _entities)
        if (e.bound.isHit(r) && e.entity.isHit(r))
            return true;
    return false;
}

bool igi::aggregate_vector::tryHit(ray &r, interaction *res) const {
    for (auto &e : _entities)
        if (e.bound.isHit(r) && e.entity.tryHit(r, res))
            return true;
    return false;
}