#pragma once

#include <functional>
#include "igiscene/IAggregateItem.h"

namespace igi {
    class aggregate {
        struct node {
            bool childIsLeaf[2];
            size_t children[2];
            bound_t bound;
        };

        struct leaf {
        };

      public:
        using allocator_type = std::pmr::polymorphic_allocator<leaf>;

        aggregate(const allocator_type &alloc)
            : _nodes(alloc), _leaves(alloc) { }
        aggregate(const aggregate &o, const allocator_type &alloc)
            : _nodes(o._nodes, alloc), _leaves(o._leaves, alloc) { }
        aggregate(aggregate &&o, const allocator_type &alloc)
            : _nodes(std::move(o._nodes), alloc), _leaves(o._leaves, alloc) { }
        aggregate(std::initializer_list<aggregate_item> il, const allocator_type &alloc)
            : aggregate(il, alloc, alloc) { }
        aggregate(std::initializer_list<aggregate_item> il, const allocator_type &alloc,
                  const allocator_type &tempAlloc);

        aggregate &operator=(const aggregate &) = delete;
        aggregate &operator=(aggregate &&) = delete;

        ~aggregate() = default;

        void add(const ISBVHItem &e);

        bool isHit(const ray &r) const;

        bool tryHit(ray &r, interaction *res) const;

      private:
        std::pmr::vector<node> _nodes;
        std::pmr::vector<leaf> _leaves;

        void initBuild(const allocator_type &alloc);
    };
}  // namespace igi