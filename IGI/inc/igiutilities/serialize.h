#pragma once

#ifdef IGI_NO_SERIALIZATION
#define RFLITE_DISABLE
#endif

#include "rflite/rflite.h"

#ifndef IGI_NO_SERIALIZATION

#include <memory_resource>
#include "igicontext.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

namespace igi {
    using serializer_t = rapidjson::Value;

    struct ser_pmr_name_a : rflite::attribute<ser_pmr_name_a> {
        std::string_view name;

        explicit constexpr ser_pmr_name_a(std::string_view name) : name(name) { }
    };

    template <typename TBase>
    using deser_pmr_func_a = rflite::func_a<TBase *(const serializer_t &)>;

    template <typename T>
    concept is_pointer_c = std::is_pointer_v<T>;

    class serialization {
      public:
        template <typename T, typename... TArgs>
        static decltype(auto) Deserialize(const serializer_t &ser, TArgs &&...args) {
            if constexpr (rflite::has_meta<T>) {
                constexpr auto ctor = rflite::meta_of<T>::attributes.template get<rflite::func_a>();

                return std::apply(
                    [=]<typename... Ts>(Ts && ...ts) { return ctor.invoke(std::forward<Ts>(ts)...); },
                    rflite::foreach_meta_of<typename decltype(ctor)::args_t>(
                        [&]<typename TMeta>(const TMeta &meta) {
                            using type = typename TMeta::class_t;

                            // exact matching of `const serializer &`
                            if constexpr (std::is_same_v<const serializer_t &, type>)
                                return std::cref(ser);
                            else {
                                constexpr size_t index = rflite::index_of_first_v<true, std::is_convertible_v<TArgs &&, type>...>;

                                // try matching by converting from `args`
                                if constexpr (index < sizeof...(TArgs))
                                    return std::get<index>(std::forward_as_tuple(std::forward<TArgs>(args)...));
                                else
                                    static_assert(sizeof(TMeta) < 0, "Unable to match some arguments");
                            }
                        }));
            }
            else {
                if constexpr (std::is_same_v<T, int>)
                    return ser.GetInt();
                else if constexpr (std::is_same_v<T, unsigned>)
                    return ser.GetUint();
                else if constexpr (std::is_same_v<T, unsigned long long>)
                    return ser.GetUint64();
                else if constexpr (std::is_same_v<T, float>)
                    return ser.GetFloat();
                else if constexpr (std::is_same_v<T, double>)
                    return ser.GetDouble();
                else if constexpr (std::is_same_v<T, const char *>)
                    return ser.GetString();
                else if constexpr (std::is_same_v<T, std::string_view>)
                    return ser.GetString();
                else
                    return;
            }
        }

        template <typename T>
        static T *DeserializePmr(const serializer_t &ser, std::string_view name) {
            std::pmr::vector<const rflite::refl_class *> children = ChildrenOf<T>();
            return DeserializePmr_impl<T>(children.begin(), children.end(), ser, name);
        }

        template <typename T, template <typename> typename TContainer, typename TNamePo>
        static TContainer<T *> DeserializePmrArray(const serializer_t &ser, TNamePo &&policy) requires std::is_convertible_v<std::invoke_result_t<TNamePo, const serializer_t &>, std::string_view> {
            if (!ser.IsArray())
                throw;

            std::pmr::vector<const rflite::refl_class *> children = ChildrenOf<T>();
            return DeserializeArray_impl<T *, TContainer>(ser, [&](const serializer_t &s) {
                return DeserializePmr_impl<T>(children.begin(), children.end(), s, policy(s));
            });
        }

        template <typename T, template <typename> typename TContainer, typename... TArgs>
        static TContainer<T> DeserializeArray(const serializer_t &ser, TArgs &&...args) {
            if (!ser.IsArray())
                throw;

            return DeserializeArray_impl<T, TContainer>(ser, [&](const serializer_t &s) {
                return Deserialize<T>(s, std::forward<TArgs>(args)...);
            });
        }

      private:
        template <typename T>
        static std::pmr::vector<const rflite::refl_class *> ChildrenOf() {
            const rflite::refl_class &base = rflite::meta_traits_rt<T>::get_meta();

            std::pmr::vector<const rflite::refl_class *> childs(base.child_count(),
                                                                context::GetTypedAllocator<const rflite::refl_class *, allocate_usage::temp>());
            base.childs(childs.begin());

            return childs;
        }

        template <typename T, typename TIt>
        static T *DeserializePmr_impl(TIt lo, TIt hi, const serializer_t &ser, std::string_view name) {
            // TODO optimization
            TIt it = std::find_if(lo, hi, [=](const rflite::refl_class *i) {
                return i->template get_attr<ser_pmr_name_a>().name == name;
            });
            if (it == hi)
                throw;

            const deser_pmr_func_a<T> &ctor = (*it)->template get_attr<deser_pmr_func_a<T>>();
            return ctor.invoke(ser);
        }

        template <typename T, template <typename> typename TContainer, typename TExPo>
        static TContainer<T> DeserializeArray_impl(const serializer_t &ser, TExPo &&policy) {
            TContainer<T> arr(ser.Size(), context::GetTypedAllocator<T>());

            auto iser = ser.Begin();
            for (auto i = arr.begin(); i != arr.end(); ++i, ++iser)
                new (&*i) T(policy(*iser));

            return arr;
        }
    };
}  // namespace igi

#define IGI_SERIALIZE_OPTIONAL(type, name, _default, ser, ...) \
    bool has_##name = (ser).HasMember(#name);                  \
    type name       = has_##name ? ::igi::serialization::Deserialize<type>((ser)[#name], __VA_ARGS__) : (_default)

#else
#define IGI_SERIALIZE_OPTIONAL(type, name, _default, ser, ...)
#endif