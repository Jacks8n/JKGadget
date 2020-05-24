#pragma once

#pragma region static reflection

#include <string_view>
#include <tuple>

#define RFLITE      ::rflite::
#define RFLITE_IMPL ::rflite::impl::

#define RFLITE_META_INFO_NAME meta_info

#define RFLITE_META_INFO_NULL_ID 0

#define RFLITE_META_INFO_NULL RFLITE_META_INFO_NAME<RFLITE_META_INFO_NULL_ID>

#define RFLITE_META_INFO_BEGIN_ID RFLITE_META_INFO_NULL::get_id()

#define RFLITE_META_INFO_BEGIN RFLITE_META_INFO_NAME<RFLITE_META_INFO_BEGIN_ID>

#define RFLITE_META_INFO_TYPE_OF(l) RFLITE_META_INFO_NAME<l>::get_meta_type()
#define RFLITE_META_INFO_TYPE_NULL  0
#define RFLITE_META_INFO_TYPE_ENTRY 1
#define RFLITE_META_INFO_TYPE_END   2

#define META_BEGIN(type, ...)                                                                                                                                          \
    template <size_t Id>                                                                                                                                               \
    struct RFLITE_META_INFO_NAME {                                                                                                                                     \
      private:                                                                                                                                                         \
        static constexpr size_t get_meta_id_impl(::std::string_view, ::std::tuple<>) noexcept {                                                                        \
            return RFLITE_META_INFO_NULL_ID;                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Nth, size_t Lo = Id>                                                                                                                          \
        static constexpr auto get_nth_meta_impl() noexcept {                                                                                                           \
            if constexpr (RFLITE_META_INFO_TYPE_OF(Lo + 1) == RFLITE_META_INFO_TYPE_ENTRY)                                                                             \
                if constexpr (Nth > 0)                                                                                                                                 \
                    return get_nth_meta_impl<Nth - 1, Lo + 2>();                                                                                                       \
                else                                                                                                                                                   \
                    return RFLITE_META_INFO_NAME<Lo + 1>();                                                                                                            \
            else if constexpr (RFLITE_META_INFO_TYPE_OF(Lo + 1) == RFLITE_META_INFO_TYPE_NULL)                                                                         \
                return get_nth_meta_impl<Nth, Lo + 1>();                                                                                                               \
            else                                                                                                                                                       \
                return RFLITE_META_INFO_NULL();                                                                                                                        \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Lo = Id, typename... Ts>                                                                                                                      \
        static constexpr auto get_all_meta_impl(::std::tuple<Ts...> t) noexcept {                                                                                      \
            constexpr auto nthinfo = get_nth_meta_impl<0, Lo>();                                                                                                       \
            if constexpr (nthinfo.get_meta_type() == RFLITE_META_INFO_TYPE_ENTRY)                                                                                      \
                return get_all_meta_impl<nthinfo.get_id() + 1>(                                                                                                        \
                    ::std::tuple<Ts..., decltype(nthinfo)>());                                                                                                         \
            else                                                                                                                                                       \
                return t;                                                                                                                                              \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T, typename... Ts>                                                                                                                          \
        static constexpr size_t get_meta_id_impl(::std::string_view member, ::std::tuple<T, Ts...> t) noexcept {                                                       \
            return T::member_name() == member ? T::get_id() : get_meta_id_impl(member, ::std::tuple<Ts...>());                                                         \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename Fn, size_t... Is>                                                                                                                           \
        static constexpr void foreach_impl(Fn &&fn, ::std::integer_sequence<bool>, ::std::index_sequence<Is...>, ::std::index_sequence<>) noexcept {                   \
            ((void)fn(get_nth_meta<Is>()), ...);                                                                                                                       \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename Fn, bool B, bool... Bs, size_t I, size_t... IL, size_t... IR>                                                                               \
        static constexpr void foreach_impl(Fn &&fn, ::std::integer_sequence<bool, B, Bs...>, ::std::index_sequence<IL...>, ::std::index_sequence<I, IR...>) noexcept { \
            constexpr ::std::integer_sequence<bool, Bs...> bseq;                                                                                                       \
            constexpr ::std::index_sequence<IR...> iseq;                                                                                                               \
            if constexpr (B)                                                                                                                                           \
                foreach_impl(::std::forward<Fn>(fn), bseq, ::std::index_sequence<IL..., I>(), iseq);                                                                   \
            else                                                                                                                                                       \
                foreach_impl(::std::forward<Fn>(fn), bseq, ::std::index_sequence<IL...>(), iseq);                                                                      \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <RFLITE member_type M, typename Fn, size_t... Is>                                                                                                     \
        static constexpr void foreach_impl(Fn &&fn, ::std::index_sequence<Is...> is) noexcept {                                                                        \
            constexpr auto seq = ::std::integer_sequence<bool, RFLITE has_flag(RFLITE member_ptr_type_v<decltype(get_nth_meta<Is>().member_ptr())>, M)...>();          \
            foreach_impl(::std::forward<Fn>(fn), seq, ::std::index_sequence<>(), is);                                                                                  \
        }                                                                                                                                                              \
                                                                                                                                                                       \
      public:                                                                                                                                                          \
        using owner_t = type;                                                                                                                                          \
                                                                                                                                                                       \
        static constexpr size_t get_meta_type() noexcept {                                                                                                             \
            return RFLITE_META_INFO_TYPE_NULL;                                                                                                                         \
        };                                                                                                                                                             \
                                                                                                                                                                       \
        static constexpr size_t get_id() noexcept {                                                                                                                    \
            return __LINE__;                                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr ::std::string_view name() noexcept {                                                                                                          \
            return #type;                                                                                                                                              \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Nth>                                                                                                                                          \
        static constexpr auto get_nth_meta() noexcept {                                                                                                                \
            return get_nth_meta_impl<Nth>();                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr auto get_all_meta() noexcept {                                                                                                                \
            return get_all_meta_impl(::std::tuple<>());                                                                                                                \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr size_t get_meta_id(::std::string_view member) noexcept {                                                                                      \
            return get_meta_id_impl(member, get_all_meta());                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr size_t get_meta_count() noexcept {                                                                                                            \
            return ::std::tuple_size_v<decltype(get_all_meta())>;                                                                                                      \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <RFLITE member_type M = RFLITE member_type::field, typename Fn>                                                                                       \
        static constexpr void foreach(Fn &&fn) noexcept {                                                                                                              \
            foreach_impl<M>(::std::forward<Fn>(fn), ::std::make_index_sequence<get_meta_count()>());                                                                   \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr auto get_attr_all() noexcept {                                                                                                                \
            return ::std::make_tuple(__VA_ARGS__);                                                                                                                     \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T>                                                                                                                                          \
        static constexpr bool has_attr() noexcept {                                                                                                                    \
            return RFLITE_IMPL tuple_has_v<decltype(get_attr_all()), T>;                                                                                               \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Nth>                                                                                                                                          \
        static constexpr auto get_nth_attr() noexcept {                                                                                                                \
            return ::std::get<Nth>(get_attr_all());                                                                                                                    \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T>                                                                                                                                          \
        static constexpr T get_attr() noexcept {                                                                                                                       \
            return ::std::get<T>(get_attr_all());                                                                                                                      \
        }                                                                                                                                                              \
    };

#define META(member, ...)                                                                                 \
    template <>                                                                                           \
    struct RFLITE_META_INFO_NAME<__LINE__> {                                                              \
        using owner_t = typename RFLITE_META_INFO_NULL::owner_t;                                          \
                                                                                                          \
        static constexpr size_t get_meta_type() noexcept {                                                \
            return RFLITE_META_INFO_TYPE_ENTRY;                                                           \
        }                                                                                                 \
                                                                                                          \
        static constexpr size_t get_id() noexcept {                                                       \
            return __LINE__;                                                                              \
        }                                                                                                 \
                                                                                                          \
        static constexpr ::std::string_view member_name() noexcept {                                      \
            return #member;                                                                               \
        }                                                                                                 \
                                                                                                          \
        static constexpr decltype(auto) member_ptr() noexcept {                                           \
            return &owner_t::member;                                                                      \
        }                                                                                                 \
                                                                                                          \
        static constexpr auto get_attr_all() noexcept {                                                   \
            return ::std::make_tuple(__VA_ARGS__);                                                        \
        }                                                                                                 \
                                                                                                          \
        template <size_t Nth>                                                                             \
        static constexpr auto get_nth_attr() noexcept {                                                   \
            return ::std::get<Nth>(get_attr_all());                                                       \
        }                                                                                                 \
                                                                                                          \
        template <typename T>                                                                             \
        static constexpr bool has_attr() noexcept {                                                       \
            return RFLITE_IMPL tuple_has_v<decltype(get_attr_all()), T>;                                  \
        }                                                                                                 \
                                                                                                          \
        template <typename T>                                                                             \
        static constexpr T get_attr() noexcept {                                                          \
            return ::std::get<T>(get_attr_all());                                                         \
        }                                                                                                 \
                                                                                                          \
        template <typename T>                                                                             \
        static constexpr bool is_type() noexcept {                                                        \
            return ::std::is_same_v<T, RFLITE_IMPL member_ptr_value_t<decltype(member_ptr())>>;           \
        }                                                                                                 \
                                                                                                          \
        static constexpr bool is_function() noexcept {                                                    \
            return ::std::is_member_function_pointer_v<decltype(member_ptr())>;                           \
        }                                                                                                 \
                                                                                                          \
        template <template <typename> typename T>                                                         \
        static constexpr bool satisfy_traits() noexcept {                                                 \
            using member_t = RFLITE_IMPL member_ptr_value_t<decltype(member_ptr())>;                      \
            return T<member_t>::value;                                                                    \
        }                                                                                                 \
                                                                                                          \
        template <typename... T>                                                                          \
        static constexpr decltype(auto) map(T &&... t) noexcept {                                         \
            return RFLITE_IMPL rflite_impl(member_ptr()).map(std::forward<T>(t)..., member_ptr());        \
        }                                                                                                 \
                                                                                                          \
        template <typename... Ts>                                                                         \
        static constexpr decltype(auto) invoke(Ts &&... ts) {                                             \
            return RFLITE_IMPL rflite_impl(member_ptr()).invoke(member_ptr(), ::std::forward<Ts>(ts)...); \
        }                                                                                                 \
    };

#define META_END                                           \
    template <>                                            \
    struct RFLITE_META_INFO_NAME<__LINE__> {               \
        static constexpr size_t get_meta_type() noexcept { \
            return RFLITE_META_INFO_TYPE_END;              \
        }                                                  \
    };

namespace rflite {
    enum member_type {
        null            = 0,
        field           = 1,
        field_static    = 2,
        function        = 4,
        function_const  = 8,
        function_static = 16,
        function_this   = function | function_const,
        any_field       = field | field_static,
        any_function    = function_this | function_static,
        any_static      = field_static | function_static,
        any             = any_field | any_function
    };

    constexpr bool has_flag(member_type type, member_type flag) {
        return type & flag;
    }

    constexpr bool is_field(member_type type) {
        return has_flag(type, member_type::any_field);
    }

    constexpr bool is_function(member_type type) {
        return has_flag(type, member_type::any_function);
    }

    constexpr bool is_static(member_type type) {
        return has_flag(type, member_type::any_static);
    }

    namespace impl {
        template <typename T>
        struct tuple_has;

        template <typename... Ts>
        struct tuple_has<::std::tuple<Ts...>> {
            template <typename T>
            static constexpr bool value = (std::is_same_v<T, Ts> || ...);
        };

        template <typename TTuple, typename T>
        static constexpr bool tuple_has_v = tuple_has<TTuple>::template value<T>;

        template <typename T>
        struct rflite_impl;

        template <typename T>
        struct rflite_impl<T *> {
            using value_t = T;

            static constexpr member_type type = member_type::field_static;

            explicit constexpr rflite_impl(T *) { }

            static constexpr decltype(auto) map(T *ptr) noexcept {
                return *ptr;
            }

            template <typename... _>
            static constexpr void map(_ &&...) noexcept { }

            template <typename... _>
            static constexpr void invoke(_ &&...) noexcept { }
        };

        template <typename T, typename C>
        struct rflite_impl<T C::*> {
            using value_t = T;
            using class_t = C;

            static constexpr member_type type = member_type::field;

            explicit constexpr rflite_impl(T C::*) { }

            template <typename U>
            static constexpr decltype(auto) map(U &&ins, T C::*ptr) noexcept {
                return ins.*ptr;
            }

            template <typename... _>
            static constexpr void invoke(_ &&...) noexcept { }
        };

        template <typename T, typename C, typename... Ts>
        struct rflite_impl<T (C::*)(Ts...)> {
            using value_t = T;
            using class_t = C;
            using args_t  = ::std::tuple<Ts...>;

            static constexpr member_type type = member_type::function;

            explicit constexpr rflite_impl(T (C::*)(Ts...)) { }

            template <typename... _>
            static constexpr void map(_ &&...) noexcept { }

            template <typename... Us>
            static constexpr decltype(auto) invoke(T (C::*ptr)(Ts...), C &ins, Us &&... us) {
                return (ins.*ptr)(::std::forward<Us>(us)...);
            }
        };

        template <typename T, typename C, typename... Ts>
        struct rflite_impl<T (C::*)(Ts...) const> {
            using value_t = T;
            using class_t = C;
            using args_t  = ::std::tuple<Ts...>;

            static constexpr member_type type = member_type::function_const;

            explicit constexpr rflite_impl(T (C::*)(Ts...) const) { }

            template <typename... _>
            static constexpr void map(_ &&...) noexcept { }

            template <typename... Us>
            static constexpr decltype(auto) invoke(T (C::*ptr)(Ts...) const, const C &ins, Us &&... us) {
                return (ins.*ptr)(::std::forward<Us>(us)...);
            }
        };

        template <typename T, typename... Ts>
        struct rflite_impl<T (*)(Ts...)> {
            using value_t = T;
            using args_t  = ::std::tuple<Ts...>;

            static constexpr member_type type = member_type::function_static;

            explicit constexpr rflite_impl(T (*)(Ts...)) { }

            template <typename... _>
            static constexpr void map(_ &&...) noexcept { }

            template <typename... Us>
            static constexpr decltype(auto) invoke(T (*ptr)(Ts...), Us &&... us) {
                return ptr(::std::forward<Us>(us)...);
            }
        };

        template <typename T>
        rflite_impl(T *) -> rflite_impl<T *>;

        template <typename T, typename C>
        rflite_impl(T C::*) -> rflite_impl<T C::*>;

        template <typename T, typename C, typename... Ts>
        rflite_impl(T (C::*)(Ts...)) -> rflite_impl<T (C::*)(Ts...)>;

        template <typename T, typename C, typename... Ts>
        rflite_impl(T (C::*)(Ts...) const) -> rflite_impl<T (C::*)(Ts...) const>;

        template <typename T, typename... Ts>
        rflite_impl(T (*)(Ts...)) -> rflite_impl<T (*)(Ts...)>;

        template <typename T>
        using member_ptr_value_t = typename rflite_impl<T>::value_t;

        template <typename T>
        using member_ptr_class_t = typename rflite_impl<T>::class_t;

        template <typename T>
        using func_ptr_args_t = typename rflite_impl<T>::args_t;

        template <typename T>
        static constexpr size_t func_ptr_args_count_v = ::std::tuple_size_v<func_ptr_args_t<T>>;

        template <typename T>
        static constexpr size_t meta_id_v = ::std::remove_reference_t<T>::template RFLITE_META_INFO_NAME<0>::get_id();
    }  // namespace impl

    template <typename T>
    static constexpr member_type member_ptr_type_v = RFLITE_IMPL rflite_impl<::std::remove_reference_t<T>>::type;

    template <typename T>
    using meta_of = typename ::std::remove_reference_t<T>::template RFLITE_META_INFO_NAME<impl::meta_id_v<T>>;

    template <size_t N, template <typename...> typename TT, typename... Ts>
    class bind_traits {
        template <size_t Lo, size_t Len>
        static constexpr auto sub_pack() noexcept {
            return sub_pack<Lo>(::std::make_index_sequence<Len>());
        }

        template <size_t Lo, size_t... Is>
        static constexpr auto sub_pack(::std::index_sequence<Is...>) noexcept {
            return ::std::tuple<::std::tuple_element_t<Lo + Is, ::std::tuple<Ts...>>...>();
        }

        template <typename... Us>
        struct impl;

        template <typename T, typename... Tslo, typename... Tshi>
        struct impl<T, ::std::tuple<Tslo...>, ::std::tuple<Tshi...>> {
            using type = TT<Tslo..., T, Tshi...>;
        };

      public:
        template <typename T>
        struct traits {
          private:
            using lo   = decltype(sub_pack<0, N>());
            using hi   = decltype(sub_pack<N, sizeof...(Ts) - N>());
            using type = typename impl<T, lo, hi>::type;

          public:
            static constexpr auto value = type::value;
        };
    };
}  // namespace rflite

#pragma region helper macro

#define GetMemberMeta(type, member)                                         \
    ([&]() constexpr {                                                      \
        constexpr size_t id = ::rflite::meta_of<type>::get_meta_id(member); \
        static_assert(id != RFLITE_META_INFO_NULL_ID, "Member not found");  \
        return type::RFLITE_META_INFO_NAME<id>();                           \
    }())

#pragma endregion
#pragma endregion

#define RFLITE_DYNAMIC

#pragma region dynamic reflection
#ifdef RFLITE_DYNAMIC

#include <memory>
#include <unordered_map>

namespace rflite::impl {
    template <typename T>
    using allocator_t = ::std::allocator<T>;

    template <typename T>
    static inline allocator_t<T> get_allocator() {
        return allocator_t<T>();
    }

    template <typename T>
    static inline T *allocate(allocator_t<T> &alloc, size_t count) noexcept {
        return ::std::allocator_traits<allocator_t<T>>::allocate(alloc, count);
    }

    template <typename T, typename... Ts>
    static inline void construct(allocator_t<T> &alloc, T *ptr, Ts &&... ts) noexcept {
        ::std::allocator_traits<allocator_t<T>>::construct(alloc, ptr, ::std::forward<Ts>(ts)...);
    }

    template <typename T>
    static inline void destroy(allocator_t<T> &alloc, T *ptr) noexcept {
        ::std::allocator_traits<allocator_t<T>>::destroy(alloc, ptr);
    }

    template <typename T>
    static inline void deallocate(allocator_t<T> &alloc, T *ptr, size_t count) noexcept {
        ::std::allocator_traits<allocator_t<T>>::deallocate(alloc, ptr, count);
    }

    static inline void *allocate(allocator_t<char> &alloc, size_t size, size_t align, size_t count) noexcept {
        size_t space = size * count;
        size_t total = space + align - 1;

        char *p  = allocate(alloc, total + sizeof(size_t));
        void *ap = p + sizeof(size_t);
        if ((ap = ::std::align(align, space, ap, total))) {
            static_cast<size_t *>(ap)[-1] = static_cast<char *>(ap) - p;
            return ap;
        }

        deallocate(alloc, p, total + sizeof(size_t));
        return nullptr;
    }

    static inline void deallocate(allocator_t<char> &alloc, void *ptr, size_t size, size_t align, size_t count) noexcept {
        if (!ptr) return;

        size_t offset = static_cast<size_t *>(ptr)[-1];
        deallocate(alloc, static_cast<char *>(ptr) - offset, size * count + align - 1 + sizeof(size_t));
    }

    template <typename T>
    static constexpr T max(const T &t) {
        return t;
    }

    template <typename T, typename... Ts>
    static constexpr T max(const T &t0, const T &t1, const Ts &... ts) {
        return max((t0 < t1) ? t1 : t0, ts...);
    }

    class any_ptr {
        struct _ { };

        mutable char _buf[impl::max(sizeof(void *), sizeof(_ _::*), sizeof(_ (_::*)()), sizeof(_ (_::*)() const))];

      public:
        template <typename T>
        any_ptr(T &&ptr) {
            *reinterpret_cast<T *>(_buf) = ptr;
        }

        template <typename T>
        T cast() const {
            return *reinterpret_cast<T *>(_buf);
        }
    };
}  // namespace rflite::impl

namespace rflite {
    class refl_member {
        template <typename T, ::std::enable_if_t<!::std::is_reference_v<T>, int> = 0>
        static constexpr decltype(auto) cast_arg_ref(void *ptr) {
            return *reinterpret_cast<T *>(ptr);
        }

        template <typename T, ::std::enable_if_t<::std::is_reference_v<T>, int> = 0>
        static constexpr decltype(auto) cast_arg_ref(void *ptr) {
            return static_cast<T>(*reinterpret_cast<::std::remove_reference_t<T> *>(ptr));
        }

        template <typename T>
        static constexpr decltype(auto) cast_res_ref(void *ptr) {
            return *reinterpret_cast<::std::remove_reference_t<impl::member_ptr_value_t<T>> *>(ptr);
        }

        template <typename T>
        static constexpr decltype(auto) cast_class_ref(void *ptr) {
            return cast_arg_ref<impl::member_ptr_class_t<T>>(ptr);
        }

        template <typename T>
        static void access_field(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
            *reinterpret_cast<impl::member_ptr_value_t<T> **>(res) = &(cast_class_ref<T>(*args).*(ptr.cast<T>()));
        }

        template <typename T>
        static void access_field_static(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
            *reinterpret_cast<T *>(res) = ptr.cast<T>();
        }

        template <typename T, typename TTuple, size_t... Is>
        struct access_func_static {
            static void impl(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
                cast_res_ref<T>(res) = ptr.cast<T>()(cast_arg_ref<::std::tuple_element_t<Is, TTuple>>(args[Is])...);
            }
        };

        template <typename T, typename TTuple, size_t... Is>
        struct access_func_member {
            static void impl(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
                cast_res_ref<T>(res) = (cast_class_ref<T>(*args).*ptr.cast<T>())(cast_arg_ref<::std::tuple_element_t<Is, TTuple>>(args[Is + 1])...);
            }
        };

        template <typename T, typename TTuple, size_t... Is>
        struct access_func_member_const {
            static void impl(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
                cast_res_ref<T>(res) = (cast_class_ref<T>(*args).*ptr.cast<T>())(cast_arg_ref<::std::tuple_element_t<Is, TTuple>>(args[Is + 1])...);
            }
        };

        template <template <typename, typename, size_t...> typename TAccess, typename T, typename TTuple, size_t... Is>
        static constexpr auto access(::std::index_sequence<Is...>) -> void (*)(void *, void **, const RFLITE_IMPL any_ptr &) {
            return &TAccess<T, TTuple, Is...>::impl;
        }

        template <typename T>
        static auto access() -> void (*)(void *, void **, const RFLITE_IMPL any_ptr &) {
            constexpr member_type type = member_ptr_type_v<T>;
            if constexpr (type == member_type::field)
                return &access_field<T>;
            if constexpr (type == member_type::field_static)
                return &access_field_static<T>;
            if constexpr (type == member_type::function_static)
                return access<access_func_static, T, RFLITE_IMPL func_ptr_args_t<T>>(::std::make_index_sequence<impl::func_ptr_args_count_v<T>>());
            if constexpr (type == member_type::function)
                return access<access_func_member, T, RFLITE_IMPL func_ptr_args_t<T>>(::std::make_index_sequence<impl::func_ptr_args_count_v<T>>());
            if constexpr (type == member_type::function_const)
                return access<access_func_member_const, T, RFLITE_IMPL func_ptr_args_t<T>>(::std::make_index_sequence<impl::func_ptr_args_count_v<T>>());
            return nullptr;
        }

        const ::std::string_view _name;

        const member_type _memberType;

        const size_t _size;

        const RFLITE_IMPL any_ptr _memberPtr;

        // when _memberType is:
        // field: return pointer to the member of instance
        // function_xxx: return invocation result of the function
        void (*const _handler)(void *, void **, const RFLITE_IMPL any_ptr &);

      public:
        template <typename T>
        refl_member(T &&meta)
            : _name(meta.member_name()),
              _memberType(member_ptr_type_v<decltype(meta.member_ptr())>),
              _size(sizeof(impl::member_ptr_value_t<decltype(meta.member_ptr())>)),
              _memberPtr(meta.member_ptr()),
              _handler(access<decltype(meta.member_ptr())>()) { }

        ::std::string_view name() const {
            return _name;
        }

        size_t size() const {
            return _size;
        }

        member_type type() const {
            return _memberType;
        }

        bool is_static() const {
            return !has_flag(member_type::any_static, _memberType);
        }

        void get(const void *ins, void *buf) const {
            assert_not_func();

            void *mp, *mut = const_cast<void *>(ins);
            _handler(&mp, &mut, _memberPtr);
            copy_impl(mp, buf);
        }

        void set(void *ins, void *buf) const {
            assert_not_func();

            void *mp;
            _handler(&mp, &ins, _memberPtr);
            copy_impl(buf, mp);
        }

        template <typename T>
        T &of(void *ins) const {
            assert_not_func();

            void *mp;
            _handler(&mp, &ins, _memberPtr);
            return *reinterpret_cast<T *>(mp);
        }

        template <typename T>
        T &of() const {
            assert_is_type(member_type::field_static);

            void *mp;
            _handler(&mp, nullptr, _memberPtr);
            return *reinterpret_cast<T *>(mp);
        }

        template <typename T>
        const T &of(const void *ins) const {
            assert_not_func();

            void *mp, *mut = const_cast<void *>(ins);
            _handler(&mp, &ins, _memberPtr);
            return *reinterpret_cast<const T *>(mp);
        }

        template <typename... Ts>
        void invoke(void *buf, Ts &&... args) const {
            invoke_impl(buf, ::std::index_sequence_for<Ts...>(), ::std::forward<Ts>(args)...);
        }

      private:
        void assert_not_func() const {
            if (is_function(_memberType))
                throw;
        }

        void assert_is_func() const {
            if (!is_function(_memberType))
                throw;
        }

        void assert_is_type(member_type type) const {
            if (_memberType != type)
                throw;
        }

        void copy_impl(void *src, void *dest) const {
            ::std::copy(static_cast<char *>(src), static_cast<char *>(src) + _size, static_cast<char *>(dest));
        }

        template <typename... Ts, size_t... Is>
        void invoke_impl(void *buf, ::std::index_sequence<Is...>, Ts &&... args) const {
            assert_is_func();

            void *paramp[sizeof...(Ts)] { (&args)... };
            _handler(buf, paramp, _memberPtr);
        }
    };

    class refl_member_view {
        friend class refl_class_view;

        const refl_member &_meta;

        void *_ins;

        refl_member_view(const refl_member &meta, void *ins)
            : _meta(meta), _ins(ins) { }

      public:
        refl_member_view &operator<<(void *buf) {
            _meta.set(_ins, buf);
            return *this;
        }

        const refl_member_view &operator>>(void *buf) const {
            _meta.get(_ins, buf);
            return *this;
        }

        template <typename T>
        T &as() {
            return _meta.of<T>(_ins);
        }

        template <typename T>
        const T &as() const {
            return _meta.of<T>(const_cast<const void *>(_ins));
        }

        template <typename... Ts>
        void invoke(void *buf, Ts &&... args) {
            if (has_flag(_meta.type(), member_type::function_this)) {
                if (_ins)
                    _meta.invoke(buf, _ins, ::std::forward<Ts>(args)...);
                else
                    throw;
            }
            else
                _meta.invoke(buf, ::std::forward<Ts>(args)...);
        }
    };

    class refl_instance;

    class refl_class {
        friend class refl_table;

        static inline RFLITE_IMPL allocator_t<char> Alloc;

        const size_t _size;

        const size_t _align;

        const ::std::shared_ptr<refl_member[]> _members;

        const size_t _count;

        refl_class(size_t size, size_t align, const ::std::shared_ptr<refl_member[]> &members, size_t count)
            : _size(size), _align(align), _members(members), _count(count) { }

      public:
        refl_class(const refl_class &) = delete;
        refl_class(refl_class &&)      = default;

        size_t size() const {
            return _size;
        }

        size_t align() const {
            return _align;
        }

        const refl_member *begin() const {
            return &_members[0];
        }

        const refl_member *end() const {
            return &_members[_count];
        }

        size_t member_count() const {
            return _count;
        }

        refl_class_view view(void *buf) const;

        refl_instance make(size_t n = 1) const;

        const refl_member &operator[](size_t index) const {
            if (index >= _count)
                throw;

            return _members[index];
        }

        const refl_member &operator[](::std::string_view member) const {
            for (const refl_member &m : *this)
                if (m.name() == member)
                    return m;
            throw;
        }
    };

    class refl_class_view {
        friend class refl_class;

        const refl_class &_meta;

        void *_ins;

        refl_class_view(const refl_class &meta, void *ins)
            : _meta(meta), _ins(ins) { }

      public:
        class iterator : ::std::iterator_traits<refl_member_view *> {
            friend class refl_class_view;

            const refl_member *_refl;

            void *_ins;

            iterator(const refl_member *refl, void *ins) : _refl(refl), _ins(ins) { }

          public:
            refl_member_view operator*() const {
                return refl_member_view(*_refl, _ins);
            }

            iterator &operator++() {
                ++_refl;
                return *this;
            }

            bool operator!=(const iterator &o) const {
                return _ins != o._ins || _refl != o._refl;
            }
        };

        iterator begin() const {
            return iterator(_meta.begin(), _ins);
        }

        iterator end() const {
            return iterator(_meta.end(), _ins);
        }

        template <typename T>
        T &as() {
            return *static_cast<T *>(_ins);
        }

        template <typename T>
        const T &as() const {
            return *static_cast<T *>(_ins);
        }

        refl_member_view operator[](size_t index) const {
            return refl_member_view(_meta[index], _ins);
        }

        refl_member_view operator[](::std::string_view member) const {
            return refl_member_view(_meta[member], _ins);
        }
    };

    class refl_instance {
        friend class refl_class;

        const refl_class &_refl;

        const size_t _size;

        const size_t _count;

        ::std::shared_ptr<void> _ins;

        refl_instance(const refl_class &refl, ::std::shared_ptr<void> ins, size_t count)
            : _refl(refl), _size(refl.size()), _count(count), _ins(::std::move(ins)) { }

      public:
        refl_instance(const refl_instance &o) = delete;
        refl_instance(refl_instance &&o)      = default;

        class iterator : ::std::iterator_traits<void *> {
            friend class refl_instance;

            const refl_class &_refl;

            void *_ins;

            size_t _size;

            iterator(const refl_class &refl, void *ins, size_t size)
                : _refl(refl), _ins(ins), _size(size) { }

          public:
            refl_class_view operator*() const {
                return _refl.view(_ins);
            }

            iterator &operator++() {
                _ins = static_cast<char *>(_ins) + _size;
                return *this;
            }

            bool operator!=(const iterator &o) const {
                return _ins != o._ins || &_refl != &o._refl;
            }
        };

        iterator begin() const {
            return iterator(_refl, at(0), _size);
        }

        iterator end() const {
            return iterator(_refl, at(_count), _size);
        }

        size_t count() const {
            return _count;
        }

        refl_class_view operator[](size_t index) const {
            if (index >= _count) throw;
            return _refl.view(at(index));
        }

      private:
        void *at(size_t index) const {
            return static_cast<char *>(_ins.get()) + index * _size;
        }
    };

    inline refl_class_view refl_class::view(void *buf) const {
        return refl_class_view(*this, buf);
    }

    inline refl_instance refl_class::make(size_t n) const {
        return refl_instance(*this,
                             ::std::shared_ptr<void>(
                                 RFLITE_IMPL allocate(Alloc, size(), align(), n),
                                 [=](void *ptr) { RFLITE_IMPL deallocate(Alloc, ptr, _size, _align, n); }),
                             n);
    }
}  // namespace rflite

namespace rflite {
    using refl_map_t = ::std::unordered_map<::std::string_view, refl_class>;

    class refl_table {
        static inline RFLITE_IMPL allocator_t<refl_member> Alloc = RFLITE_IMPL get_allocator<refl_member>();

        static inline refl_map_t ClassMap { static_cast<typename refl_map_t::allocator_type>(Alloc) };

      public:
        template <typename T>
        static std::string_view regist() {
            if constexpr (meta_of<T>::template has_attr<const char *>())
                return regist<T>(meta_of<T>::template get_attr<const char *>());
            else
                return regist<T>(meta_of<T>::name());
        }

        template <typename T>
        static std::string_view regist(::std::string_view id) {
            constexpr size_t nrefl = meta_of<T>::get_meta_count();

            ::std::shared_ptr<refl_member[]> ap(
                RFLITE_IMPL allocate(Alloc, nrefl),
                [=](refl_member *p) {
                    RFLITE_IMPL destroy(Alloc, p);
                    RFLITE_IMPL deallocate(Alloc, p, nrefl);
                });

            size_t i = 0;
            meta_of<T>::template foreach<member_type::any>([&](auto &&meta) {
                RFLITE_IMPL construct(Alloc, &ap[i++], meta);
            });

            if (ClassMap.emplace(id, refl_class(sizeof(T), alignof(T), ap, nrefl)).second)
                return id;
            throw;
        }

        static const refl_class &get_class(::std::string_view name) {
            return ClassMap.at(name);
        }
    };
}  // namespace rflite

#define META_END_RT \
    META_END        \
    static inline const std::string_view meta_info_rt = ::rflite::refl_table::regist<typename RFLITE_META_INFO_NULL::owner_t>();

#endif
#pragma endregion
