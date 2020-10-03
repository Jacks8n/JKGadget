#pragma once

#include <memory_resource>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rflite/rflite.h"

namespace igi {
    using serializer_t = rapidjson::Value;

    template <typename T>
    using ser_allocator_t = std::pmr::polymorphic_allocator<T>;

    struct ser_pmr_name_a : rflite::attribute<ser_pmr_name_a> {
        std::string_view name;

        explicit constexpr ser_pmr_name_a(std::string_view name) : name(name) { }
    };

    using deser_pmr_allocator_t = ser_allocator_t<char>;

    template <typename TBase>
    using deser_pmr_func_a = rflite::func_a<TBase *(const serializer_t &, const deser_pmr_allocator_t &)>;

    template <typename T>
    concept is_pointer_c = std::is_pointer_v<T>;

    template <typename T>
    concept is_std_allocator_c = requires(T &_) {
        { _.allocate(0) }
        ->is_pointer_c;
    };

    class serialization {
        static constexpr void *allocator_null = nullptr;

      public:
        template <rflite::has_meta T, typename TAlloc, typename... TArgs>
        static decltype(auto) Deserialize(const serializer_t &ser, TAlloc &&alloc, TArgs &&... args) {
            constexpr auto ctor = rflite::meta_of<T>::attributes.template get<rflite::func_a>();

            return std::apply(
                [=]<typename... Ts>(Ts && ... ts) { return ctor.invoke(std::forward<Ts>(ts)...); },
                rflite::foreach_meta_of<typename decltype(ctor)::args_t>(
                    [&]<typename TMeta>(const TMeta &meta) {
                        using type = typename TMeta::class_t;

                        // 1st priority: exact matching of `const serializer &`
                        if constexpr (std::is_same_v<const serializer_t &, type>)
                            return std::cref(ser);
                        // 2nd priority: any allocator satisfying `is_std_allocator_c`
                        else if constexpr (is_std_allocator_c<std::remove_cvref_t<type>>)
                            return static_cast<type>(alloc);
                        else {
                            constexpr size_t index = rflite::index_of_first_v<true, std::is_convertible_v<TArgs &&, type>...>;

                            // 3rd priority: try converting from `args`
                            if constexpr (index < sizeof...(TArgs))
                                return std::get<index>(std::forward_as_tuple(std::forward<TArgs>(args)...));
                            else
                                static_assert(sizeof(TMeta) < 0, "Unable to match some arguments");
                        }
                    }));
        }

        template <typename T>
        static decltype(auto) Deserialize(const serializer_t &ser) {
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
                return Deserialize<T>(ser, allocator_null);
        }

        template <typename T, typename TAlloc>
        static T *DeserializePmr(const serializer_t &ser, TAlloc &&alloc, std::string_view name) {
            std::pmr::vector<const rflite::refl_class *> children = ChildrenOf<T>(alloc);
            return DeserializePmr_impl<T>(children.begin(), children.end(), ser, std::forward<TAlloc>(alloc), name);
        }

        template <typename T, template <typename> typename TContainer, typename TAlloc, typename TNamePo>
        static TContainer<T *> DeserializePmrArray(const serializer_t &ser, TAlloc &&alloc, TNamePo &&policy) requires std::is_convertible_v<std::invoke_result_t<TNamePo, const serializer_t &>, std::string_view> {
            if (!ser.IsArray())
                throw;

            std::pmr::vector<const rflite::refl_class *> children = ChildrenOf<T>(alloc);
            return DeserializeArray_impl<T *, TContainer>(ser, alloc, [&](const serializer_t &s) {
                return DeserializePmr_impl<T>(children.begin(), children.end(), s, alloc, policy(s));
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
            // TODO optimization
            TIt it = std::find_if(lo, hi, [=](const rflite::refl_class *i) {
                return i->template get_attr<ser_pmr_name_a>().name == name;
            });
            if (it == hi)
                throw;

            const deser_pmr_func_a<T> &ctor = (*it)->template get_attr<deser_pmr_func_a<T>>();
            return ctor.invoke(ser, alloc);
        }

        template <typename T, template <typename> typename TContainer, typename TAlloc, typename TExPo>
        static TContainer<T> DeserializeArray_impl(const serializer_t &ser, TAlloc &&alloc, TExPo &&policy) {
            TContainer<T> arr(ser.Size(), alloc);
            for (auto i = ser.Begin(); i != ser.End(); ++i)
                arr.emplace_back(policy(*i));
            return arr;
        }
    };
}  // namespace igi

#define IGI_SERIALIZE_OPTIONAL(type, name, _default, ser, ...) \
    bool has_##name = (ser).HasMember(#name);                  \
    type name       = has_##name ? ::igi::serialization::Deserialize<type>((ser)[#name], __VA_ARGS__) : (_default)
