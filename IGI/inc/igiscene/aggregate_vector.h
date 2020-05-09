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
        aggregate_vector(std::initializer_list<aggregate_elem> il, const allocator_type &alloc)
            : _entities(il, alloc) { }
        aggregate_vector(std::initializer_list<entity> il, const allocator_type &alloc)
            : _entities(make_aggregate_elem_getter(il.begin()),
                        make_aggregate_elem_getter(il.end()), alloc) { }

        aggregate_vector &operator=(const aggregate_vector &) = delete;
        aggregate_vector &operator=(aggregate_vector &&) = delete;

        ~aggregate_vector() = default;

        void add(const aggregate_elem &e) override {
            _entities.push_back(e);
        }

        bool isHit(const ray &r) const override;

        bool tryHit(ray &r, interaction *res) const override;

      private:
        std::pmr::vector<aggregate_elem> _entities;
    };
}  // namespace igi