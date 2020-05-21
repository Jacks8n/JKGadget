#pragma once

#include <string_view>
#include <tuple>

#define REFL_META_INFO_NAME meta_info

#define REFL_META_INFO_NULL_ID 0

#define REFL_META_INFO_NULL REFL_META_INFO_NAME<REFL_META_INFO_NULL_ID>

#define REFL_META_INFO_BEGIN_ID REFL_META_INFO_NULL::get_id()

#define REFL_META_INFO_BEGIN REFL_META_INFO_NAME<REFL_META_INFO_BEGIN_ID>

#define REFL_META_INFO_OWNER_T REFL_META_INFO_NULL::owner_t

#define REFL_META_INFO_TYPE(l)    REFL_META_INFO_NAME<l>::get_meta_type()
#define REFL_META_INFO_TYPE_NULL  0
#define REFL_META_INFO_TYPE_ENTRY 1
#define REFL_META_INFO_TYPE_END   2

#define META_BEGIN(type, ...)                                                      \
    template <size_t _MId>                                                         \
    struct REFL_META_INFO_NAME {                                                   \
        using owner_t = type;                                                      \
                                                                                   \
        static constexpr size_t get_meta_type() noexcept {                         \
            return REFL_META_INFO_TYPE_NULL;                                       \
        };                                                                         \
                                                                                   \
        static constexpr size_t get_id() noexcept {                                \
            return _MId;                                                           \
        }                                                                          \
                                                                                   \
        template <size_t Nth>                                                      \
        static constexpr auto get_nth_meta() noexcept {                            \
            return get_nth_meta_impl<Nth>();                                       \
        }                                                                          \
                                                                                   \
        template <typename...>                                                     \
        static constexpr auto get_all_meta() noexcept {                            \
            return get_all_meta_impl(std::tuple<>());                              \
        }                                                                          \
                                                                                   \
        template <typename...>                                                     \
        static constexpr size_t get_meta_id(std::string_view member) noexcept {    \
            return get_meta_id_impl(member, get_all_meta<>());                     \
        }                                                                          \
                                                                                   \
        template <typename...>                                                     \
        static constexpr size_t get_meta_count() noexcept {                        \
            return std::tuple_size_v<decltype(get_all_meta<>())>;                  \
        }                                                                          \
                                                                                   \
        template <typename Fn>                                                     \
        static constexpr void foreach_meta(Fn &&fn) noexcept {                     \
            foreach_meta_impl(fn, std::make_index_sequence<get_meta_count<>()>()); \
        }                                                                          \
                                                                                   \
        static constexpr auto get_attr_all() noexcept {                            \
            return std::make_tuple(__VA_ARGS__);                                   \
        }                                                                          \
                                                                                   \
        template <size_t Nth>                                                      \
        static constexpr auto get_nth_attr() noexcept {                            \
            return std::get<Nth>(get_attr_all());                                  \
        }                                                                          \
                                                                                   \
        template <typename T>                                                      \
        static constexpr T get_attr() noexcept {                                   \
            return std::get<T>(get_attr_all());                                    \
        }                                                                          \
                                                                                   \
      private:                                                                     \
        template <size_t Nth, size_t Lo = _MId>                                    \
        static constexpr auto get_nth_meta_impl() noexcept {                       \
            if constexpr (REFL_META_INFO_TYPE(Lo + 1)                              \
                          == REFL_META_INFO_TYPE_ENTRY)                            \
                if constexpr (Nth > 0)                                             \
                    return get_nth_meta_impl<Nth - 1, Lo + 2>();                   \
                else                                                               \
                    return REFL_META_INFO_NAME<Lo + 1>();                          \
            else if constexpr (REFL_META_INFO_TYPE(Lo + 1)                         \
                               == REFL_META_INFO_TYPE_NULL)                        \
                return get_nth_meta_impl<Nth, Lo + 1>();                           \
            else                                                                   \
                return REFL_META_INFO_NULL();                                      \
        }                                                                          \
                                                                                   \
        template <size_t Lo = _MId, typename... Ts>                                \
        static constexpr auto get_all_meta_impl(std::tuple<Ts...> t) noexcept {    \
            constexpr auto nthinfo = get_nth_meta_impl<0, Lo>();                   \
            if constexpr (nthinfo.get_meta_type() == REFL_META_INFO_TYPE_ENTRY)    \
                return get_all_meta_impl<nthinfo.get_id() + 1>(                    \
                    std::tuple<Ts..., decltype(nthinfo)>());                       \
            else                                                                   \
                return t;                                                          \
        }                                                                          \
                                                                                   \
        static constexpr size_t get_meta_id_impl(                                  \
            std::string_view, std::tuple<>) noexcept {                             \
            return REFL_META_INFO_NULL_ID;                                         \
        }                                                                          \
                                                                                   \
        template <typename T, typename... Ts>                                      \
        static constexpr size_t get_meta_id_impl(                                  \
            std::string_view member, std::tuple<T, Ts...> t) noexcept {            \
            return T::member_name() == member                                      \
                       ? T::get_id()                                               \
                       : get_meta_id_impl(member, std::tuple<Ts...>());            \
        }                                                                          \
                                                                                   \
        template <typename Fn, size_t... Is>                                       \
        static constexpr void foreach_meta_impl(                                   \
            Fn &&fn, std::index_sequence<Is...>) noexcept {                        \
            ((void)fn(get_nth_meta<Is>()), ...);                                   \
        }                                                                          \
    };

#define META(member, ...)                                                                 \
    template <>                                                                           \
    struct REFL_META_INFO_NAME<__LINE__> {                                                \
        using owner_t = REFL_META_INFO_NULL::owner_t;                                     \
                                                                                          \
        static constexpr size_t get_meta_type() noexcept {                                \
            return REFL_META_INFO_TYPE_ENTRY;                                             \
        }                                                                                 \
                                                                                          \
        static constexpr size_t get_id() noexcept {                                       \
            return __LINE__;                                                              \
        }                                                                                 \
                                                                                          \
        static constexpr auto get_attr_all() noexcept {                                   \
            return std::make_tuple(__VA_ARGS__);                                          \
        }                                                                                 \
                                                                                          \
        static constexpr std::string_view member_name() noexcept {                        \
            return #member;                                                               \
        }                                                                                 \
                                                                                          \
        static constexpr auto member_ptr() noexcept {                                     \
            return &REFL_META_INFO_BEGIN::owner_t::member;                                \
        }                                                                                 \
                                                                                          \
        template <size_t Nth>                                                             \
        static constexpr auto get_nth_attr() noexcept {                                   \
            return std::get<Nth>(get_attr_all());                                         \
        }                                                                                 \
                                                                                          \
        template <typename T>                                                             \
        static constexpr T get_attr() noexcept {                                          \
            return std::get<T>(get_attr_all());                                           \
        }                                                                                 \
                                                                                          \
        static constexpr auto &get_ref_of(owner_t &ins) noexcept {                        \
            return ins.*member_ptr();                                                     \
        }                                                                                 \
                                                                                          \
        static constexpr const auto &get_ref_of(const owner_t &ins) noexcept {            \
            return ins.*member_ptr();                                                     \
        }                                                                                 \
                                                                                          \
        template <typename T>                                                             \
        static constexpr bool is_type() noexcept {                                        \
            return std::is_same_v<T,                                                      \
                                  ::reflitest::impl::member_ptr_res_t<                    \
                                      decltype(member_ptr())>>;                           \
        }                                                                                 \
                                                                                          \
        template <template <typename> typename T>                                         \
        static constexpr bool satisfy_traits() noexcept {                                 \
            return T<::reflitest::impl::member_ptr_res_t<decltype(member_ptr())>>::value; \
        }                                                                                 \
                                                                                          \
        template <typename... Ts>                                                         \
        static auto apply(const owner_t &ins, Ts... ts) noexcept {                        \
            return get_ref_of(ins)(std::forward<Ts>(ts)...);                              \
        }                                                                                 \
                                                                                          \
        template <typename... Ts>                                                         \
        static auto apply(owner_t &ins, Ts... ts) noexcept {                              \
            constexpr auto ptr = member_ptr();                                            \
                                                                                          \
            using member_t = ::reflitest::impl::member_ptr_res_t<decltype(ptr)>;          \
                                                                                          \
            if constexpr (std::is_invocable_v<member_t, Ts...>)                           \
                return (ins.*ptr)(std::forward<Ts>(ts)...);                               \
            else                                                                          \
                ins.*ptr = member_t(std::forward<Ts>(ts)...);                             \
        }                                                                                 \
                                                                                          \
        template <typename... Ts>                                                         \
        static auto assign(owner_t &ins, Ts... ts) noexcept {                             \
            constexpr auto ptr = member_ptr();                                            \
                                                                                          \
            using member_t = ::reflitest::impl::member_ptr_res_t<decltype(ptr)>;          \
                                                                                          \
            ins.*ptr = member_t(std::forward<Ts>(ts)...);                                 \
        }                                                                                 \
    };

#define META_END                                           \
    template <>                                            \
    struct REFL_META_INFO_NAME<__LINE__> {                 \
        static constexpr size_t get_meta_type() noexcept { \
            return REFL_META_INFO_TYPE_END;                \
        }                                                  \
    };

namespace reflitest::impl {
    template <typename T>
    struct member_ptr_res { };

    template <typename T, typename M>
    struct member_ptr_res<T M::*> {
        using type = T;
    };

    template <typename T, typename M>
    struct member_ptr_res<T M::*const> {
        using type = T;
    };

    template <typename T>
    using member_ptr_res_t = typename member_ptr_res<T>::type;
}  // namespace reflitest::impl

namespace reflitest {
    template <typename T>
    using meta_of = typename std::remove_reference_t<T>::template meta_info<
        std::remove_reference_t<T>::template meta_info<0>::get_id()>;

    template <size_t N, template <typename...> typename TT, typename... Ts>
    class bind_traits {
        template <size_t Lo, size_t Len>
        static constexpr auto SubTypePack() noexcept {
            return SubTypePack<Lo>(std::make_index_sequence<Len>());
        }

        template <size_t Lo, size_t... Is>
        static constexpr auto SubTypePack(std::index_sequence<Is...>) noexcept {
            return std::tuple<std::tuple_element_t<Lo + Is, std::tuple<Ts...>>...>();
        }

        template <typename... Us>
        struct impl;

        template <typename T, typename... Tslo, typename... Tshi>
        struct impl<T, std::tuple<Tslo...>, std::tuple<Tshi...>> {
            using type = TT<Tslo..., T, Tshi...>;
        };

      public:
        template <typename T>
        struct traits {
            using type = typename impl<T, decltype(SubTypePack<0, N>()),
                                       decltype(SubTypePack<N, sizeof...(Ts) - N>())>::type;

            static constexpr auto value = type::value;
        };
    };
}  // namespace reflitest

#define GetMemberMeta(type, member)                                              \
    ([&]() constexpr {                                                           \
        constexpr size_t id = ::reflitest::meta_of<type>::get_meta_id<>(member); \
        static_assert(id != REFL_META_INFO_NULL_ID, "Member not found");         \
        return type::REFL_META_INFO_NAME<id>();                                  \
    }())

#define GetMemberType(meta) ::reflitest::impl::member_ptr_res_t<decltype((meta).member_ptr())>

//===============================================
//              Dynamic reflection
//===============================================

#define REFLITEST_DYNAMIC 1

#if REFLITEST_DYNAMIC

#include <vector>

#define META_END_RT \
    META_END        \
    static inline auto meta_info_rt = ::reflitest::refl_table::regist(this);

namespace reflitest::impl {
    struct empty { };

    using any_member_ptr_t = void *empty::*;

    using any_member_func_ptr_t = void (*empty::*)();

    struct refl_member_entry {
        const std::string_view name;

        const std::string_view type;

        const any_member_ptr_t member_ptr;

        template <typename T>
        refl_member_entry(T &&meta)
            : name(meta.member_name()), member_ptr(meta.member_ptr()) { }
    };

    struct refl_class_entry {
        const std::vector<refl_member_entry> members;

        const any_member_func_ptr_t ctor;

        refl_class_entry(std::vector<refl_member_entry> &&members, any_member_func_ptr_t ctor)
            : members(std::move(members)), ctor(ctor) { }
    };
}  // namespace reflitest::impl
#endif

#if REFLITEST_DYNAMIC == 1
#include <unordered_map>
namespace reflitest::impl {
    using refl_map_t = std::unordered_map<std::string_view, refl_class_entry>;
}
#elif REFLITE_DYNAMIC == 2
#include <map>
namespace reflitest::impl {
    using refl_map_t = std::map<std::string_view, std::vector<impl::any_member_ptr_t>>;
}
#endif  // REFLITE_DYNAMIC

#if REFLITEST_DYNAMIC
namespace reflitest {
    class refl_table {
        static inline std::allocator<impl::refl_member_entry> _alloc;

        static inline impl::refl_map_t _memberMap { _alloc };

      public:
        template <typename T>
        static impl::empty regist(const T *_) {
            regist(meta_of<T>::get_attr<const char *>(), _);
        }

        template <typename T>
        static impl::empty regist(std::string_view id, const T *) {
            constexpr size_t nrefl = metaof<T>::get_member_count<>();

            std::vector<member_refl_pair> ap(nrefl, _alloc);
            meta_of<T>::foreach_meta([&](auto &&meta) {
                ap.emplace_back(meta);
            });

            _memberMap.emplace(id, impl::refl_class_entry( std::move(ap));
        }

        static void *construct(std::string_view name, void *buf) {
            const impl::refl_class_entry &entry = _memberMap.at(name);
            (reinterpret_cast<impl::empty *>(buf)->*entry.ctor)();
        }
    };
}  // namespace reflitest
#endif