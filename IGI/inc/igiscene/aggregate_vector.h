#pragma once

#include <vector>
#include "igiscene/IAggregate.h"

namespace igi {
    class aggregate_vector : public IAggregate {
        std::vector<entity*> _entities;

    public:
        aggregate_vector() = default;
        aggregate_vector(const aggregate_vector&) = default;
        aggregate_vector(aggregate_vector&&) = default;
        aggregate_vector(std::initializer_list<entity*> entities) : _entities(entities) {}

        aggregate_vector& operator=(const aggregate_vector&) = default;
        aggregate_vector& operator=(aggregate_vector&&) = default;

        ~aggregate_vector() = default;

        void add(entity& e) override {
            _entities.push_back(&e);
        }

        void addRange(size_t n, entity* es) override {
            _entities.insert(_entities.end(), &es, &es + n);
        }

        bool isHit(const ray& r) const override;

        bool tryHit(ray& r, interaction& res) const override;
    };
}