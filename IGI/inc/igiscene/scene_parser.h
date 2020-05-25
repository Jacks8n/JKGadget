#pragma once

#include "igiscene/scene.h"
#include "rapidjson/document.h"

namespace igi {
    class scene_parser {
      public:
        using parse_res_t    = std::pair<scene, std::shared_ptr<aggregate>>;
        using allocator_type = typename aggregate::allocator_type;

        template <typename TIStream>
        static parse_res_t ParseStream(TIStream &&input, const allocator_type &alloc) {
            rapidjson::Document doc;
            return Parse(doc.ParseStream(std::forward<TIStream>(input)), alloc);
        }

        static parse_res_t ParseInsitu(char *buf, const allocator_type &alloc) {
            rapidjson::Document doc;
            return Parse(doc.ParseInsitu(buf), alloc);
        }

        static parse_res_t Parse(rapidjson::Document &doc, const allocator_type &alloc);
    };
}  // namespace igi
