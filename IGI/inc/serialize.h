#pragma once

#include <memory_resource>
#include "rapidjson/document.h"
#include "rflite/rflite.h"

namespace igi {
    using serializer_t = rapidjson::Value;

    class serialization {
      public:
        template <typename T, typename TIStream, typename TAlloc>
        static T DeserializeStream(TIStream &&input, TAlloc &&alloc) {
            rapidjson::Document doc;

            return Deserialize<T>(doc.ParseStream(std::forward<TIStream>(input)).GetObject(),
                                  std::forward<TAlloc>(alloc));
        }

        template <typename T, typename TAlloc>
        static T DeserializeInsitu(char *buf, TAlloc &&alloc) {
            rapidjson::Document doc;

            return Deserialize<T>(doc.ParseInsitu(buf).GetObject(), std::forward<TAlloc>(alloc));
        }

        template <typename T, typename TAlloc, typename... TArgs>
        static T Deserialize(const serializer_t &ser, TAlloc &&alloc, TArgs &&... args) {
            using meta_t = rflite::meta_of<T>;
            static_assert(meta_t::template has_attr<rflite::ctor_a>(), "constructor attribute is required to auto deserialize");

            constexpr auto ctor = meta_t::template get_attr<rflite::ctor_a>();
            return std::apply(ctor.construct,
                              rflite::meta_of_foreach<typename decltype(ctor)::args_t>([&](auto &&meta) {
                                  using type = std::remove_reference_t<decltype(meta)>;
                                  if constexpr (std::is_same_v<rapidjson::Document, type>)
                                      return std::ref(ser);
                                  else if constexpr (std::is_constructible_v<TAlloc &&, type>)
                                      return type(alloc);
                                  else {
                                      constexpr size_t index = rflite::index_of_first_t_v<type, std::remove_reference_t<TArgs>...>;
                                      if constexpr (index < sizeof...(TArgs))
                                          return std::get<index>(std::forward_as_tuple(args...));
                                      else
                                          return Deserialize<rflite::meta_of<decltype(meta)>::owner_t>(ser, alloc);
                                  }
                              }));
        }

        template <typename T, template <typename> typename TContainer = std::pmr::vector, typename TAlloc, typename... TArgs>
        static TContainer<T> DeserializeArray(const serializer_t &ser, TAlloc &alloc, TArgs &&... args) {
            assert(ser.IsArray());

            TContainer<T> arr(ser.Size(), alloc);
            for (auto i = ser.Begin(); i != ser.End(); ++i)
                arr.push_back(Deserialize<T>(*i, alloc, std::forward<TArgs>(args)...);

            return arr;
        }
    };
}  // namespace igi
