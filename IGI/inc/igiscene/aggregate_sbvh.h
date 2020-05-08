#pragma once

#include "igiacceleration/sbvh.h"
#include "igiscene/IAggregate.h"

namespace igi {
    class aggregate_sbvh : public IAggregate {
        struct nodes { };

        struct leaves { };

      public:
        using allocator_type = std::pmr::polymorphic_allocator<entity>;

        aggregate_sbvh(const allocator_type &alloc)
            : _nodes(alloc), _leaves(alloc) { }
        aggregate_sbvh(const aggregate_sbvh &o, const allocator_type &alloc)
            : _nodes(o._nodes, alloc), _leaves(o._leaves, alloc) { }
        aggregate_sbvh(aggregate_sbvh &&o, const allocator_type &alloc)
            : _nodes(std::move(o._nodes), alloc), _leaves(o._leaves, alloc) { }
        aggregate_sbvh(std::initializer_list<entity *> entities, const allocator_type &alloc);

        aggregate_sbvh &operator=(const aggregate_sbvh &) = delete;
        aggregate_sbvh &operator=(aggregate_sbvh &&) = delete;

        ~aggregate_sbvh() = default;

        void add(entity &e) override;

        void addRange(size_t n, entity *es) override {
            while (n--)
                add(*es++);
        }

        bool isHit(const ray &r) const override;

        bool tryHit(ray &r, interaction &res) const override;

      private:
        std::pmr::vector<nodes> _nodes;
        std::pmr::vector<leaves> _leaves;
    };
}  // namespace igi