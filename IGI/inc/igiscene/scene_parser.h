#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "reflitest.h"

namespace igi {
    class scene_parser {
        template <typename T>
        void parse(const rapidjson::Document &json, T *res) {
            const rapidjson::Value &obj = json[reflitest::meta_of<T>::get_attr<std::string_view>()];

            meta_of<T>::foreach_meta([&](auto &&meta) {
                const rapidjson::Value &prop = obj[meta.get_attr<std::string_view>()];

                auto &field = meta.get_ref_of(*res);
                if constexpr (meta.satisfy_traits<std::is_integral>())
                    field = prop.GetInt();
                else if constexpr (meta.satisfy_traits<std::is_floating_point>())
                    field = prop.GetFloat();
                else if constexpr (meta.satisfy_traits<std::is_class>())
                    parse(json, &field);
            });
        }
    };
}  // namespace igi
