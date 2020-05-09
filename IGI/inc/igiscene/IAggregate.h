#pragma once

#include "igientity/entity.h"
#include "igigeometry/bound.h"
#include "igigeometry/ray.h"
#include "igimaterial/interaction.h"

namespace igi {
    struct aggregate_elem {
        const entity &entity;
        bound_t bound;
    };

    template <typename It>
    class aggregate_elem_getter {
        It _it;

      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = typename std::iterator_traits<It>::value_type;
        using difference_type   = typename std::iterator_traits<It>::difference_type;
        using pointer           = typename std::iterator_traits<It>::pointer;
        using reference         = typename std::iterator_traits<It>::reference;

        aggregate_elem_getter() = default;
        explicit aggregate_elem_getter(const It &it) : _it(it) { }

        aggregate_elem operator*() {
            const entity &e = *_it;
            return aggregate_elem { e, e.getBound() };
        }

        aggregate_elem_getter &operator++() {
            ++_it;
            return *this;
        }

        bool operator!=(const aggregate_elem_getter &o) const {
            return _it != o._it;
        }
    };

    template <typename It>
    inline aggregate_elem_getter<It> make_aggregate_elem_getter(It &&it) {
        return aggregate_elem_getter<It>(it);
    }

    struct IAggregate {
        virtual void add(const aggregate_elem &e) = 0;

        template <typename It>
        void addRange(It &&it, size_t n) {
            while (n--)
                add(*it++);
        }

        virtual bool isHit(const ray &r) const              = 0;
        virtual bool tryHit(ray &r, interaction *res) const = 0;
    };
}  // namespace igi
