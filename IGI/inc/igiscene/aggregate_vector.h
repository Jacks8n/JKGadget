#pragma once

#include "igiscene/IAggregate.h"

namespace igi {
    class aggregate_vector : public IAggregate {
      public:
        using allocator_type = std::pmr::polymorphic_allocator<entity>;

        aggregate_vector(const allocator_type &alloc)
            : _entities(alloc) { }
        aggregate_vector(const aggregate_vector &o, const allocator_type &alloc)
            : _entities(o._entities, alloc) { }
        aggregate_vector(aggregate_vector &&o, const allocator_type &alloc)
            : _entities(std::move(o._entities), alloc) { }
        aggregate_vector(std::initializer_list<entity *> entities, const allocator_type &alloc)
            : _entities(entities, alloc) { }

        aggregate_vector &operator=(const aggregate_vector &) = delete;
        aggregate_vector &operator=(aggregate_vector &&) = delete;

        ~aggregate_vector() = default;

        void add(entity &e) override {
            _entities.push_back(&e);
        }

        void addRange(size_t n, entity *es) override {
            _entities.insert(_entities.end(), &es, &es + n);
        }

        bool isHit(const ray &r) const override;

        bool tryHit(ray &r, interaction &res) const override;

      private:
        std::pmr::vector<entity *> _entities;
    };
}  // namespace igi