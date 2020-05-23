#pragma once

#define REFLITEST_DYNAMIC 1

#pragma region static reflection

#include <string_view>
#include <tuple>

#define REFL_META_INFO_NAME meta_info

#define REFL_META_INFO_NULL_ID 0

#define REFL_META_INFO_NULL REFL_META_INFO_NAME<REFL_META_INFO_NULL_ID>

#define REFL_META_INFO_BEGIN_ID REFL_META_INFO_NULL::get_id()

#define REFL_META_INFO_BEGIN REFL_META_INFO_NAME<REFL_META_INFO_BEGIN_ID>

#define REFL_META_INFO_TYPE_OF(l) REFL_META_INFO_NAME<l>::get_meta_type()
#define REFL_META_INFO_TYPE_NULL  0
#define REFL_META_INFO_TYPE_ENTRY 1
#define REFL_META_INFO_TYPE_END   2

#define META_BEGIN(type, ...)                                                                                \
    template <size_t _MId>                                                                                   \
    struct REFL_META_INFO_NAME {                                                                             \
      private:                                                                                               \
        static constexpr size_t get_meta_id_impl(std::string_view, std::tuple<>) noexcept {                  \
            return REFL_META_INFO_NULL_ID;                                                                   \
        }                                                                                                    \
                                                                                                             \
        template <size_t Nth, size_t Lo = _MId>                                                              \
        static constexpr auto get_nth_meta_impl() noexcept {                                                 \
            if constexpr (REFL_META_INFO_TYPE_OF(Lo + 1) == REFL_META_INFO_TYPE_ENTRY)                       \
                if constexpr (Nth > 0)                                                                       \
                    return get_nth_meta_impl<Nth - 1, Lo + 2>();                                             \
                else                                                                                         \
                    return REFL_META_INFO_NAME<Lo + 1>();                                                    \
            else if constexpr (REFL_META_INFO_TYPE_OF(Lo + 1) == REFL_META_INFO_TYPE_NULL)                   \
                return get_nth_meta_impl<Nth, Lo + 1>();                                                     \
            else                                                                                             \
                return REFL_META_INFO_NULL();                                                                \
        }                                                                                                    \
                                                                                                             \
        template <size_t Lo = _MId, typename... Ts>                                                          \
        static constexpr auto get_all_meta_impl(std::tuple<Ts...> t) noexcept {                              \
            constexpr auto nthinfo = get_nth_meta_impl<0, Lo>();                                             \
            if constexpr (nthinfo.get_meta_type() == REFL_META_INFO_TYPE_ENTRY)                              \
                return get_all_meta_impl<nthinfo.get_id() + 1>(                                              \
                    std::tuple<Ts..., decltype(nthinfo)>());                                                 \
            else                                                                                             \
                return t;                                                                                    \
        }                                                                                                    \
                                                                                                             \
        template <typename T, typename... Ts>                                                                \
        static constexpr size_t get_meta_id_impl(std::string_view member, std::tuple<T, Ts...> t) noexcept { \
            return T::member_name() == member ? T::get_id() : get_meta_id_impl(member, std::tuple<Ts...>()); \
        }                                                                                                    \
                                                                                                             \
        template <typename Fn, size_t... Is>                                                                 \
        static constexpr void foreach_meta_impl(Fn &&fn, std::index_sequence<Is...>) noexcept {              \
            ((void)fn(get_nth_meta<Is>()), ...);                                                             \
        }                                                                                                    \
                                                                                                             \
      public:                                                                                                \
        using owner_t = type;                                                                                \
                                                                                                             \
        static constexpr size_t get_meta_type() noexcept {                                                   \
            return REFL_META_INFO_TYPE_NULL;                                                                 \
        };                                                                                                   \
                                                                                                             \
        static constexpr size_t get_id() noexcept {                                                          \
            return _MId;                                                                                     \
        }                                                                                                    \
                                                                                                             \
        template <size_t Nth>                                                                                \
        static constexpr auto get_nth_meta() noexcept {                                                      \
            return get_nth_meta_impl<Nth>();                                                                 \
        }                                                                                                    \
                                                                                                             \
        static constexpr auto get_all_meta() noexcept {                                                      \
            return get_all_meta_impl(std::tuple<>());                                                        \
        }                                                                                                    \
                                                                                                             \
        static constexpr size_t get_meta_id(std::string_view member) noexcept {                              \
            return get_meta_id_impl(member, get_all_meta());                                                 \
        }                                                                                                    \
                                                                                                             \
        static constexpr size_t get_meta_count() noexcept {                                                  \
            return std::tuple_size_v<decltype(get_all_meta())>;                                              \
        }                                                                                                    \
                                                                                                             \
        template <typename Fn>                                                                               \
        static constexpr void foreach_meta(Fn &&fn) noexcept {                                               \
            foreach_meta_impl(fn, std::make_index_sequence<get_meta_count()>());                             \
        }                                                                                                    \
                                                                                                             \
        static constexpr auto get_attr_all() noexcept {                                                      \
            return std::make_tuple(__VA_ARGS__);                                                             \
        }                                                                                                    \
                                                                                                             \
        template <size_t Nth>                                                                                \
        static constexpr auto get_nth_attr() noexcept {                                                      \
            return std::get<Nth>(get_attr_all());                                                            \
        }                                                                                                    \
                                                                                                             \
        template <typename T>                                                                                \
        static constexpr T get_attr() noexcept {                                                             \
            return std::get<T>(get_attr_all());                                                              \
        }                                                                                                    \
    };

#define META(member, ...)                                                                            \
    template <>                                                                                      \
    struct REFL_META_INFO_NAME<__LINE__> {                                                           \
        using owner_t = typename REFL_META_INFO_NULL::owner_t;                                       \
                                                                                                     \
        static constexpr size_t                                                                      \
        get_meta_type() noexcept {                                                                   \
            return REFL_META_INFO_TYPE_ENTRY;                                                        \
        }                                                                                            \
                                                                                                     \
        static constexpr size_t get_id() noexcept {                                                  \
            return __LINE__;                                                                         \
        }                                                                                            \
                                                                                                     \
        static constexpr auto get_attr_all() noexcept {                                              \
            return std::make_tuple(__VA_ARGS__);                                                     \
        }                                                                                            \
                                                                                                     \
        static constexpr std::string_view member_name() noexcept {                                   \
            return #member;                                                                          \
        }                                                                                            \
                                                                                                     \
        static constexpr decltype(auto) member_ptr() noexcept {                                      \
            return &owner_t::member;                                                                 \
        }                                                                                            \
                                                                                                     \
        template <size_t Nth>                                                                        \
        static constexpr auto get_nth_attr() noexcept {                                              \
            return std::get<Nth>(get_attr_all());                                                    \
        }                                                                                            \
                                                                                                     \
        template <typename T>                                                                        \
        static constexpr T get_attr() noexcept {                                                     \
            return std::get<T>(get_attr_all());                                                      \
        }                                                                                            \
                                                                                                     \
        template <typename T>                                                                        \
        static constexpr bool is_type() noexcept {                                                   \
            return std::is_same_v<T, ::reflitest::impl::member_ptr_value_t<decltype(member_ptr())>>; \
        }                                                                                            \
                                                                                                     \
        static constexpr bool is_function() noexcept {                                               \
            return std::is_member_function_pointer_v<decltype(member_ptr())>;                        \
        }                                                                                            \
                                                                                                     \
        template <template <typename> typename T>                                                    \
        static constexpr bool satisfy_traits() noexcept {                                            \
            using member_t = ::reflitest::impl::member_ptr_value_t<decltype(member_ptr())>;          \
            return T<member_t>::value;                                                               \
        }                                                                                            \
                                                                                                     \
      public:                                                                                        \
        static constexpr decltype(auto) of(owner_t &ins) noexcept {                                  \
            return ::reflitest::impl::access_impl(member_ptr()).of(ins, member_ptr());               \
        }                                                                                            \
                                                                                                     \
        static constexpr decltype(auto) of(const owner_t &ins) noexcept {                            \
            return ::reflitest::impl::access_impl(member_ptr()).of(ins, member_ptr());               \
        }                                                                                            \
                                                                                                     \
        template <typename... Ts>                                                                    \
        static constexpr decltype(auto) invoke(owner_t &ins, Ts &&... ts) {                          \
            return ::reflitest::impl::access_impl(member_ptr())                                      \
                .invoke(ins, member_ptr(), std::forward<Ts>(ts)...);                                 \
        }                                                                                            \
                                                                                                     \
        template <typename... Ts>                                                                    \
        static constexpr decltype(auto) invoke(const owner_t &ins, Ts &&... ts) {                    \
            return ::reflitest::impl::access_impl(member_ptr())                                      \
                .invoke(ins, member_ptr(), std::forward<Ts>(ts)...);                                 \
        }                                                                                            \
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
    struct member_ptr_traits { };

    template <typename T, typename M>
    struct member_ptr_traits<T M::*> {
        using value_t = T;
        using class_t = M;

        static constexpr bool is_function = false;
    };

    template <typename T, typename M>
    struct member_ptr_traits<T M::*const> {
        using value_t = T;
        using class_t = M;

        static constexpr bool is_function = false;
    };

    template <typename T, typename M, typename... Ts>
    struct member_ptr_traits<T (M::*const)(Ts...)> {
        using value_t = T;
        using class_t = M;
        using args_t  = std::tuple<Ts...>;

        static constexpr bool is_function = true;
    };

    template <typename T>
    using member_ptr_value_t = typename member_ptr_traits<T>::value_t;

    template <typename T>
    using member_ptr_class_t = typename member_ptr_traits<T>::class_t;

    template <typename T>
    using member_ptr_args_t = typename member_ptr_traits<T>::args_t;

    template <typename T>
    static constexpr bool is_member_function_v = member_ptr_traits<T>::is_function;

    template <typename T>
    struct access_impl;

    template <typename T, typename M>
    struct access_impl<T M::*> {
        explicit constexpr access_impl(T M::*) { }

        template <typename U>
        static constexpr decltype(auto) of(U &&ins, T M::*ptr) noexcept {
            return ins.*ptr;
        }

        template <typename... _>
        static constexpr void invoke(_ &&...) noexcept { }
    };

    template <typename T, typename M, typename... Ts>
    struct access_impl<T (M::*)(Ts...)> {
        explicit constexpr access_impl(T (M::*)(Ts...)) { }

        template <typename... _>
        static constexpr void of(_ &&...) noexcept { }

        template <typename... Us>
        static constexpr decltype(auto) invoke(M &ins, T (M::*ptr)(Ts...), Us &&... us) {
            return (ins.*ptr)(std::forward<Us>(us)...);
        }
    };

    template <typename T, typename M, typename... Ts>
    struct access_impl<T (M::*)(Ts...) const> {
        explicit constexpr access_impl(T (M::*)(Ts...) const) { }

        template <typename... _>
        static constexpr void of(_ &&...) noexcept { }

        template <typename... Us>
        static constexpr decltype(auto) invoke(const M &ins, T (M::*ptr)(Ts...) const, Us &&... us) {
            return (ins.*ptr)(std::forward<Us>(us)...);
        }
    };

    template <typename T, typename M>
    access_impl(T M::*) -> access_impl<T M::*>;

    template <typename T, typename M, typename... Ts>
    access_impl(T (M::*)(Ts...)) -> access_impl<T (M::*)(Ts...)>;

    template <typename T, typename M, typename... Ts>
    access_impl(T (M::*)(Ts...) const) -> access_impl<T (M::*)(Ts...) const>;

    template <typename T>
    static constexpr size_t meta_id_v = std::remove_reference_t<T>::template meta_info<0>::get_id();
}  // namespace reflitest::impl

namespace reflitest {
    template <typename T>
    using meta_of = typename std::remove_reference_t<T>::template meta_info<impl::meta_id_v<T>>;

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
          private:
            using lo   = decltype(SubTypePack<0, N>());
            using hi   = decltype(SubTypePack<N, sizeof...(Ts) - N>());
            using type = typename impl<T, lo, hi>::type;

          public:
            static constexpr auto value = type::value;
        };
    };
}  // namespace reflitest

#pragma region helper macro

#define GetMemberMeta(type, member)                                            \
    ([&]() constexpr {                                                         \
        constexpr size_t id = ::reflitest::meta_of<type>::get_meta_id(member); \
        static_assert(id != REFL_META_INFO_NULL_ID, "Member not found");       \
        return type::REFL_META_INFO_NAME<id>();                                \
    }())

#define GetMemberType(meta) ::reflitest::impl::member_ptr_value_t<decltype((meta).member_ptr())>

#pragma endregion
#pragma endregion

#pragma region dynamic reflection
#if REFLITEST_DYNAMIC

#include <cstdlib>
#include <memory>

#define REFL_ALLOC(type) std::allocator<type>

namespace reflitest::impl {
    template <typename T>
    using allocator_t = REFL_ALLOC(T);

    template <typename T>
    static inline allocator_t<T> get_allocator() {
        return allocator_t<T>();
    }

    template <typename T>
    static inline T *allocate(allocator_t<T> &alloc, size_t count) noexcept {
        return std::allocator_traits<allocator_t<T>>::allocate(alloc, count);
    }

    template <typename T, typename... Ts>
    static inline void construct(allocator_t<T> &alloc, T *ptr, Ts &&... ts) noexcept {
        std::allocator_traits<allocator_t<T>>::construct(alloc, ptr, std::forward<Ts>(ts)...);
    }

    template <typename T>
    static inline void destroy(allocator_t<T> &alloc, T *ptr) noexcept {
        std::allocator_traits<allocator_t<T>>::destroy(alloc, ptr);
    }

    template <typename T>
    static inline void deallocate(allocator_t<T> &alloc, T *ptr, size_t count) noexcept {
        std::allocator_traits<allocator_t<T>>::deallocate(alloc, ptr, count);
    }

    static inline void *allocate(allocator_t<char> &alloc, size_t size, size_t align, size_t count) noexcept {
        size_t space = size * count;
        size_t total = space + align - 1;

        char *p  = allocate(alloc, total + sizeof(size_t));
        void *ap = p + sizeof(size_t);
        if ((ap = std::align(align, space, ap, total))) {
            static_cast<size_t *>(ap)[-1] = static_cast<char *>(ap) - p;
            return ap;
        }

        deallocate(alloc, p, total + sizeof(size_t));
        return nullptr;
    }

    static inline void deallocate(allocator_t<char> &alloc, void *ptr, size_t size, size_t align, size_t count) noexcept {
        size_t offset = static_cast<size_t *>(ptr)[-1];
        deallocate(alloc, static_cast<char *>(ptr) - offset, size * count + align - 1 + sizeof(size_t));
    }

    struct placeholder { };

    using any_member_ptr_t = void *placeholder::*;
}  // namespace reflitest::impl

namespace reflitest {
    class refl_member {
        friend class REFL_ALLOC(refl_member);

        const std::string_view _name;

        void *(*const _handler)(void *, const impl::any_member_ptr_t &);

        const impl::any_member_ptr_t _memberPtr;

        const size_t _size;

        const bool _isFunc;

        template <typename T>
        static void *access(void *ins, const impl::any_member_ptr_t &mp) {
            T ptr = reinterpret_cast<T>(mp);
            return &(reinterpret_cast<impl::member_ptr_class_t<T> *>(ins)->*ptr);
        }

      public:
        template <typename T>
        refl_member(T &&meta)
            : _name(meta.member_name()),
              _handler(&access<decltype(meta.member_ptr())>),
              _memberPtr(reinterpret_cast<impl::any_member_ptr_t>(meta.member_ptr())),
              _size(sizeof(impl::member_ptr_value_t<decltype(meta.member_ptr())>)),
              _isFunc(meta.is_function()) { }

        bool is_function() const {
            return _isFunc;
        }

        std::string_view name() const {
            return _name;
        }

        size_t size() const {
            return _size;
        }

        void get(const void *ins, void *buf) const {
            assertNotFunc();

            void *mp = _handler(const_cast<void *>(ins), _memberPtr);
            memcpy(buf, mp, _size);
        }

        void set(void *ins, void *buf) const {
            assertNotFunc();

            void *mp = _handler(const_cast<void *>(ins), _memberPtr);
            memcpy(mp, buf, _size);
        }

        template <typename T>
        const T &from(const void *ins) const {
            assertNotFunc();

            void *mp = _handler(const_cast<void *>(ins), _memberPtr);
            return *reinterpret_cast<const T *>(mp);
        }

        template <typename T>
        T &from(void *ins) const {
            assertNotFunc();

            void *mp = _handler(const_cast<void *>(ins), _memberPtr);
            return *reinterpret_cast<T *>(mp);
        }

      private:
        void assertNotFunc() const {
#if _DEBUG
            if (is_function()) throw;
#endif
        }

        void assertIsFunc() const {
#if _DEBUG
            if (!is_function()) throw;
#endif
        }
    };

    class refl_member_view {
        friend class refl_class_view;

        const refl_member &_meta;

        void *_ins;

        refl_member_view(const refl_member &meta, void *ins) : _meta(meta), _ins(ins) { }

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
            return _meta.from<T>(_ins);
        }

        template <typename T>
        const T &as() const {
            return _meta.from<T>(const_cast<const void *>(_ins));
        }
    };

    class refl_class {
        friend class refl_table;

        static inline impl::allocator_t<char> Alloc;

        const size_t _size;

        const size_t _align;

        const std::shared_ptr<refl_member[]> _members;

        const size_t _count;

        refl_class(size_t size, size_t align, const std::shared_ptr<refl_member[]> &members, size_t count)
            : _size(size), _align(align), _members(members), _count(count) { }

      public:
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

        size_t count() const {
            return _count;
        }

        refl_class_view view(void *buf) const;

        void *allocate(size_t n = 1) const noexcept {
            void *buf = impl::allocate(Alloc, size(), align(), n);
            if (buf)
                return buf;

            return nullptr;
        }

        void deallocate(void *ptr, size_t n = 1) const noexcept {
            impl::deallocate(Alloc, ptr, size(), align(), n);
        }

        const refl_member &operator[](size_t index) const {
#if _DEBUG
            if (index >= _count) throw;
#endif
            return _members[index];
        }

        const refl_member &operator[](std::string_view member) const {
            for (const refl_member &m : *this)
                if (m.name() == member)
                    return m;
            throw;
        }
    };

    class refl_class_view {
        friend class refl_class;

        const refl_class &_meta;

        void *const _buf;

        refl_class_view(const refl_class &meta, void *buf) : _meta(meta), _buf(buf) { }

      public:
        refl_member_view operator[](size_t index) const {
            return refl_member_view(_meta[index], _buf);
        }

        refl_member_view operator[](std::string_view member) const {
            return refl_member_view(_meta[member], _buf);
        }

        template <typename T>
        T &as() {
            return *static_cast<T *>(_buf);
        }

        template <typename T>
        const T &as() const {
            return *static_cast<T *>(_buf);
        }
    };

    inline refl_class_view refl_class::view(void *buf) const {
        return refl_class_view(*this, buf);
    }
}  // namespace reflitest

#if REFLITEST_DYNAMIC == 1

#include <unordered_map>

namespace reflitest::impl {
    using refl_map_t = std::unordered_map<std::string_view, refl_class>;
}  // namespace reflitest::impl

#elif REFLITEST_DYNAMIC == 2

#include <map>

namespace reflitest::impl {
    using refl_map_t = std::map<std::string_view, std::vector<impl::any_member_ptr_t>>;
}  // namespace reflitest::impl

#else

static_assert(false, "Invalid map type");

#endif  // REFLITE_DYNAMIC

namespace reflitest {
    class refl_table {
        static inline impl::allocator_t<refl_member> Alloc = impl::get_allocator<refl_member>();

        static inline impl::refl_map_t ClassMap { static_cast<typename impl::refl_map_t::allocator_type>(Alloc) };

      public:
        template <typename T>
        static void regist() {
            regist<T>(meta_of<T>::template get_attr<const char *>());
        }

        template <typename T>
        static void regist(std::string_view id) {
            constexpr size_t nrefl = meta_of<T>::get_meta_count();

            std::shared_ptr<refl_member[]> ap(
                impl::allocate(Alloc, nrefl),
                [&](refl_member *p) {
                    impl::destroy(Alloc, p);
                    impl::deallocate(Alloc, p, nrefl);
                });

            size_t i = 0;
            meta_of<T>::foreach_meta([&](auto &&meta) { impl::construct(Alloc, &ap[i++], meta); });

            ClassMap.emplace(id, refl_class(sizeof(T), alignof(T), ap, nrefl));
        }

        static const refl_class &get_class(std::string_view name) {
            return ClassMap.at(name);
        }
    };
}  // namespace reflitest

#define META_END_RT \
    META_END        \
    static inline size_t meta_info_rt = (::reflitest::refl_table::regist<typename REFL_META_INFO_NULL::owner_t>(), 0);

#undef REFL_ALLOC

#endif
#pragma endregion
