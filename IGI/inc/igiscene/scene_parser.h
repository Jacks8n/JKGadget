#pragma once

#include "igiscene/scene.h"
#include "rapidjson/document.h"
#include "rflite/rflite.h"

namespace igi {
    class scene_parser {
      public:
        using allocator_type = typename aggregate::allocator_type;

        template <typename TIStream>
        static scene ParseStream(TIStream &&input, const allocator_type &alloc) {
            rapidjson::Document doc;
            return Parse<scene>(doc.ParseStream(std::forward<TIStream>(input)), alloc);
        }

        static scene ParseInsitu(char *buf, const allocator_type &alloc) {
            rapidjson::Document doc;
            return Parse<scene>(doc.ParseInsitu(buf), alloc);
        }

        template <typename T>
        static T Parse(rapidjson::Document &doc, const allocator_type &alloc) {
            using meta_t = rflite::meta_of<T>;
            static_assert(meta_t::template has_attr<rflite::ctor_a>(), "constructor attribute is required to auto deserialize");

            constexpr auto ctor = meta_t::template get_attr<rflite::ctor_a>();
            return std::apply(ctor.construct,
                              rflite::meta_of_foreach<typename decltype(ctor)::args_t>([&](auto &&meta) {
                                  return Parse<rflite::meta_of<decltype(meta)>::owner_t>(doc, alloc);
                              }));
        }
    };
}  // namespace igi
