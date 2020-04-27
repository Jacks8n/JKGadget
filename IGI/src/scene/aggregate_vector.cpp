#include "igiscene/aggregate_vector.h"

bool igi::aggregate_vector::isHit(const ray& r) const {
    for (entity* e : _entities)
        if (e->isHit(r))
            return true;
    return false;
}

bool igi::aggregate_vector::tryHit(ray& r, interaction& res) const {
    for (entity* e : _entities)
        if (e->tryHit(r, res))
            return true;
    return false;
}