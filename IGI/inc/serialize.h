#pragma once

#include <memory_resource>
#include "rapidjson/document.h"
#include "rflite/rflite.h"

namespace igi {
    using serializer_t = rapidjson::Value;

    struct serialize_name_a : rflite::attribute<serialize_name_a> {
        std::string_view name;

        explicit constexpr serialize_name_a(std::string_view name) : name(name) { }
    };

    class serialization {
      public:
        using allocator_type = std::pmr::polymorphic_allocator<char>;

        template <typename T>
        using pure_pm_deser_a = rflite::func_a<T *(const serializer_t &, const allocator_type &)>;

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

        template <rflite::has_meta T, typename TAlloc, typename... TArgs>
        static decltype(auto) Deserialize(const serializer_t &ser, TAlloc &&alloc, TArgs &&... args) {
            constexpr auto ctor = rflite::meta_of<T>::attributes.template get<rflite::func_a>();

            return std::apply(
                [=]<typename... Ts>(Ts && ... ts) { return ctor.invoke(std::forward<Ts>(ts)...); },
                rflite::foreach_meta_of<typename decltype(ctor)::args_t>(
                    [&]<typename TMeta>(const TMeta &meta) {
                        using type = rflite::remove_null_meta_t<TMeta>;
                        if constexpr (rflite::is_null_meta_v<TMeta>) {
                            if constexpr (std::is_same_v<const serializer_t &, type>)
                                return std::cref(ser);
                            else if constexpr (std::is_convertible_v<decltype(alloc), type>)
                                return std::cref(alloc);
                            else {
                                constexpr size_t index = rflite::index_of_first_v<true, std::is_convertible_v<type, TArgs &&>...>;
                                if constexpr (index < sizeof...(TArgs))
                                    return std::get<index>(std::forward_as_tuple(std::forward<TArgs>(args)...));
                                else
                                    static_assert(sizeof(TMeta) < 0, "Unable to match some arguments");
                            }
                        }
                        else
                            static_assert(sizeof(TMeta) < 0, "Unable to match some arguments");
                    }));
        }

        template <typename T, typename TAlloc>
        static T *DeserializePmr(const serializer_t &ser, TAlloc &&alloc, std::string_view name) {
            std::pmr::vector<const rflite::refl_class *> children = ChildrenOf<T>(alloc);
            return DeserializePmr_impl(children.begin(), children.end(), ser, std::forward<TAlloc>(alloc), name);
        }

        template <typename T, template <typename> typename TContainer, typename TAlloc, typename TNamePo>
        static TContainer<T *> DeserializePmrArray(const serializer_t &ser, TAlloc &&alloc, TNamePo &&policy) requires std::is_convertible_v<std::invoke_result_t<TNamePo, const serializer_t &>, std::string_view> {
            if (!ser.IsArray())
                throw;

            std::pmr::vector<const rflite::refl_class *> children = ChildrenOf<T>(alloc);
            return DeserializeArray_impl<T *, TContainer>(ser, alloc, [&](const serializer_t &s) {
                return DeserializePmr_impl<T>(children.begin(), children.end(), ser, alloc, policy(s));
            });
        }

        template <typename T, template <typename> typename TContainer, typename TAlloc, typename... TArgs>
        static TContainer<T> DeserializeArray(const serializer_t &ser, TAlloc &&alloc, TArgs &&... args) {
            if (!ser.IsArray())
                throw;

            return DeserializeArray_impl<T, TContainer>(ser, alloc, [&](const serializer_t &s) {
                return Deserialize<T>(s, alloc, std::forward<TArgs>(args)...);
            });
        }

      private:
        template <typename T, typename TAlloc>
        static std::pmr::vector<const rflite::refl_class *> ChildrenOf(TAlloc &&alloc) {
            const rflite::refl_class &base = rflite::meta_traits_rt<T>::get_meta();

            std::pmr::vector<const rflite::refl_class *> childs(base.child_count(), alloc);
            base.childs(childs.begin());

            return childs;
        }

        template <typename T, typename TIt, typename TAlloc>
        static T *DeserializePmr_impl(TIt lo, TIt hi, const serializer_t &ser, TAlloc &&alloc, std::string_view name) {
            TIt it = std::find_if(lo, hi, [&](const rflite::refl_class *i) {
                return i->template get_attr<serialize_name_a>().name == name;
            });
            if (it == hi)
                throw;

            auto ctor = (*it)->template get_attr<pure_pm_deser_a<T>>();
            return ctor.invoke(ser, static_cast<allocator_type>(alloc));
        }

        template <typename T, template <typename> typename TContainer, typename TAlloc, typename TExPo>
        static TContainer<T> DeserializeArray_impl(const serializer_t &ser, TAlloc &&alloc, TExPo &&policy) {
            TContainer<T> arr(ser.Size(), alloc);
            for (auto i = ser.Begin(); i != ser.End(); ++i)
                arr.push_back(policy(*i));
            return arr;
        }
    };

    template <typename T>
    using pure_pm_deser_a = typename serialization::template pure_pm_deser_a<T>;
}  // namespace igi
