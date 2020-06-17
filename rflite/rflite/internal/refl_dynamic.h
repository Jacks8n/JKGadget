#pragma once

#ifndef RFLITE_DYNAMIC_DISABLE

#ifndef RFLITE_PREPROCESS_ONLY
#include <assert.h>
#include <memory>
#include <unordered_map>
#include <vector>
#endif

#include "rflite/internal/refl_static.h"

RFLITE_IMPL_NS {
    template <typename T>
    using allocator_type = ::std::allocator<T>;

    template <typename T>
    static inline allocator_type<T> get_allocator() {
        return allocator_type<T>();
    }

    template <typename T>
    static inline auto get_allocator_of_type() {
        return typename T::allocator_type();
    }

    template <typename T>
    static inline T *allocate(allocator_type<T> & alloc, size_t count) noexcept {
        return ::std::allocator_traits<allocator_type<T>>::allocate(alloc, count);
    }

    template <typename T, typename... Ts>
    static inline void construct(allocator_type<T> & alloc, T * ptr, Ts && ... ts) noexcept {
        ::std::allocator_traits<allocator_type<T>>::construct(alloc, ptr, ::std::forward<Ts>(ts)...);
    }

    template <typename T>
    static inline void destroy(allocator_type<T> & alloc, T * ptr) noexcept {
        ::std::allocator_traits<allocator_type<T>>::destroy(alloc, ptr);
    }

    template <typename T>
    static inline void deallocate(allocator_type<T> & alloc, T * ptr, size_t count) noexcept {
        ::std::allocator_traits<allocator_type<T>>::deallocate(alloc, ptr, count);
    }

    static inline void *allocate(allocator_type<char> & alloc, size_t size, size_t align, size_t count) noexcept {
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

    static inline void deallocate(allocator_type<char> & alloc, void *ptr, size_t size, size_t align, size_t count) noexcept {
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
        mutable char _buf[max(sizeof(_ *), sizeof(_ (*)()), sizeof(_ _::*), sizeof(_ (_::*)()), sizeof(_ (_::*)() const))];

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
}

RFLITE_NS {
    class refl_class;
    class refl_instance;

    using refl_map_t  = ::std::unordered_map<::std::string_view, refl_class>;
    using refl_iter_t = typename refl_map_t::iterator;

    using refl_child_collection_t = ::std::vector<::std::string_view>;

    class refl_attr_collection {
        friend class refl_class;
        friend class refl_member;

        using indexed_attr_t = ::std::pair<typename attribute_tag::id_t, const attribute_tag *>;

        static inline RFLITE_IMPL allocator_type<indexed_attr_t> Alloc = RFLITE_IMPL get_allocator<indexed_attr_t>();

        indexed_attr_t *_attrs;

        const size_t _nattr;

        template <typename... Ts, size_t... Is>
        refl_attr_collection(const ::std::tuple<Ts...> &t, ::std::index_sequence<Is...>)
            : _attrs(sizeof...(Is) > 0 ? RFLITE_IMPL allocate(Alloc, sizeof...(Ts)) : nullptr), _nattr(sizeof...(Ts)) {
            ((RFLITE_IMPL construct(Alloc, _attrs + Is, attribute_tag::template get_id<Ts>(), &::std::get<Is>(t))), ...);
        }

        template <typename... Ts>
        refl_attr_collection(const ::std::tuple<Ts...> &t)
            : refl_attr_collection(t, ::std::index_sequence_for<Ts...>()) { }

      public:
        refl_attr_collection(const refl_attr_collection &) = delete;
        refl_attr_collection(refl_attr_collection &&o) : _attrs(o._attrs), _nattr(o._nattr) {
            o._attrs = nullptr;
        }

        ~refl_attr_collection() {
            RFLITE_IMPL deallocate(Alloc, _attrs, _nattr);
        }

        template <typename T>
        const T &get() const requires(!RFLITE_IMPL typed_attribute<RFLITE_IMPL attr_wrap_t<T>>) {
            using wrap_t = RFLITE_IMPL attr_wrap_t<T>;
            return *static_cast<const wrap_t *>(get_impl<wrap_t>());
        }

        template <RFLITE_IMPL typed_attribute T, typename TClass>
        const typename T::template typed<TClass> &get() const {
            return *static_cast<const T::template typed<TClass> *>(get_impl<T>());
        }

      private:
        template <typename T>
        const attribute_tag *get_impl() const {
            size_t id = attribute_tag::template get_id<T>();

            const indexed_attr_t *tag = ::std::find_if(_attrs, _attrs + _nattr,
                                                       [=](const indexed_attr_t &t) { return t.first == id; });
            if (tag < _attrs + _nattr)
                return tag->second;
            throw;
        }
    };

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
            return *reinterpret_cast<::std::remove_reference_t<member_ptr_value_t<T>> *>(ptr);
        }

        template <typename T>
        static constexpr decltype(auto) cast_class_ref(void *ptr) {
            return cast_arg_ref<member_ptr_class_t<T>>(ptr);
        }

        template <typename T>
        static void access_field(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
            *reinterpret_cast<member_ptr_value_t<T> **>(res) = &(cast_class_ref<T>(*args).*(ptr.cast<T>()));
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
            constexpr member_type type = RFLITE member_ptr_type_v<T>;
            if constexpr (type == member_type::field)
                return &access_field<T>;
            if constexpr (type == member_type::field_static)
                return &access_field_static<T>;
            if constexpr (type == member_type::function_static)
                return access<access_func_static, T, RFLITE func_ptr_args_t<T>>(::std::make_index_sequence<RFLITE func_ptr_args_count_v<T>>());
            if constexpr (type == member_type::function)
                return access<access_func_member, T, RFLITE func_ptr_args_t<T>>(::std::make_index_sequence<RFLITE func_ptr_args_count_v<T>>());
            if constexpr (type == member_type::function_const)
                return access<access_func_member_const, T, RFLITE func_ptr_args_t<T>>(::std::make_index_sequence<RFLITE func_ptr_args_count_v<T>>());
            return nullptr;
        }

        const ::std::string_view _name;

        refl_attr_collection _attrs;

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
              _attrs(meta.attributes.all()),
              _memberType(RFLITE member_ptr_type_v<decltype(meta.member_ptr())>),
              _size(sizeof(RFLITE member_ptr_value_t<decltype(meta.member_ptr())>)),
              _memberPtr(meta.member_ptr()),
              _handler(access<decltype(meta.member_ptr())>()) { }

        ::std::string_view name() const {
            return _name;
        }

        template <typename T>
        decltype(auto) get_attr() const {
            return _attrs.get<T>();
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
            assert(_memberType == member_type::field_static);

            void *mp;
            _handler(&mp, nullptr, _memberPtr);
            return *reinterpret_cast<T *>(mp);
        }

        template <typename T>
        const T &of(const void *ins) const {
            assert_not_func();

            void *mp, *mut = const_cast<void *>(ins);
            _handler(&mp, const_cast<void **>(&ins), _memberPtr);
            return *reinterpret_cast<const T *>(mp);
        }

        template <typename... Ts>
        void invoke(void *buf, Ts &&... args) const {
            invoke_impl(buf, ::std::index_sequence_for<Ts...>(), ::std::forward<Ts>(args)...);
        }

      private:
        void assert_not_func() const {
            assert(!is_function(_memberType));
        }

        void assert_is_func() const {
            assert(is_function(_memberType));
        }

        void copy_impl(void *src, void *dest) const {
            ::std::copy(static_cast<char *>(src), static_cast<char *>(src) + _size, static_cast<char *>(dest));
        }

        template <typename... Ts, size_t... Is>
        void invoke_impl(void *buf, ::std::index_sequence<Is...>, Ts &&... args) const {
            assert_is_func();

            void *param[sizeof...(Ts)] { (&args)... };
            _handler(buf, param, _memberPtr);
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
                assert(_ins);
                _meta.invoke(buf, _ins, ::std::forward<Ts>(args)...);
            }
            else
                _meta.invoke(buf, ::std::forward<Ts>(args)...);
        }
    };

    class refl_class {
        friend class refl_table;

        static inline RFLITE_IMPL allocator_type<char> Alloc;

        const ::std::string_view _name;

        const size_t _size;

        const size_t _align;

        refl_attr_collection _attrs;

        ::std::shared_ptr<refl_member[]> _members;

        const size_t _nmember;

        const bool _isBase;

        const refl_iter_t _baseIt;

        refl_child_collection_t _childs;

        template <typename TAttrTuple>
        refl_class(::std::string_view name, size_t size, size_t align, const TAttrTuple &attrs,
                   ::std::shared_ptr<refl_member[]> members, size_t nmember,
                   bool isBase, const refl_iter_t &baseIt)
            : _name(name), _size(size), _align(align), _attrs(attrs),
              _members(::std::move(members)), _nmember(nmember),
              _isBase(isBase), _baseIt(baseIt),
              _childs(RFLITE_IMPL get_allocator_of_type<refl_child_collection_t>()) {
            if (!isBase)
                baseIt->second._childs.push_back(name);
        }

      public:
        refl_class(const refl_class &) = delete;
        refl_class(refl_class &&)      = default;

        ::std::string_view name() const {
            return _name;
        }

        size_t size() const {
            return _size;
        }

        size_t align() const {
            return _align;
        }

        template <typename T>
        decltype(auto) get_attr() const {
            return _attrs.get<T>();
        }

        const refl_class &base() const {
            if (_isBase)
                return *this;
            else
                return _baseIt->second;
        }

        const refl_member *begin() const {
            return &_members[0];
        }

        const refl_member *end() const {
            return &_members[_nmember];
        }

        size_t member_count() const {
            return _nmember;
        }

        refl_class_view view(void *buf) const;

        refl_instance make(size_t n = 1) const;

        template <typename TInsIt>
        void childs(TInsIt &&it) const;

        size_t child_count() const {
            return _childs.size();
        }

        const refl_member &operator[](size_t index) const {
            assert(index < _nmember);
            return _members[index];
        }

        const refl_member &operator[](::std::string_view member) const {
            for (const refl_member &m : *this)
                if (m.name() == member)
                    return m;
            throw;
        }

        bool operator==(const refl_class &r) const {
            return &r == this;
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

        const refl_class &meta() const {
            return _meta;
        }

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
            assert(index < _count);
            return _refl.view(at(index));
        }

      private:
        void *at(size_t index) const {
            return static_cast<char *>(_ins.get()) + index * _size;
        }
    };

    class refl_table {
        static inline RFLITE_IMPL allocator_type<refl_member> Alloc = RFLITE_IMPL get_allocator<refl_member>();

        static inline refl_map_t ClassMap { RFLITE_IMPL get_allocator_of_type<refl_map_t>() };

      public:
        template <typename T>
        static constexpr std::string_view name_of() noexcept {
            if constexpr (meta_of<T>::attributes.template has<name_a>())
                return meta_of<T>::attributes.template get<name_a>().name;
            else
                return meta_of<T>::name();
        }

        template <typename T>
        static refl_iter_t regist() {
            return regist<T>(name_of<T>());
        }

        template <typename T>
        static refl_iter_t regist(::std::string_view name) {
            if (is_registered(name))
                return ClassMap.find(name);

            refl_iter_t base;
            if constexpr (!is_base_v<T>)
                base = regist<base_of<T>>();

            constexpr size_t nrefl = meta_of<T>::get_meta_count();

            ::std::shared_ptr<refl_member[]> members(
                nrefl ? RFLITE_IMPL allocate(Alloc, nrefl) : nullptr,
                [=](refl_member *p) {
                    if constexpr (nrefl > 0) {
                        RFLITE_IMPL destroy(Alloc, p);
                        RFLITE_IMPL deallocate(Alloc, p, nrefl);
                    }
                });

            size_t i = 0;
            meta_of<T>::template foreach<member_type::any>([&](auto &&meta) {
                RFLITE_IMPL construct(Alloc, &members[i++], meta);
            });

            constexpr const auto &attrs = meta_of<T>::attributes.all();
            refl_class rc(name, sizeof(T), alignof(T), attrs, members, nrefl, is_base_v<T>, base);

            auto [it, res] = ClassMap.emplace(name, ::std::move(rc));
            if (res)
                return it;
            throw;
        }

        static const refl_class &get_class(::std::string_view name) {
            return ClassMap.at(name);
        }

      private:
        static bool is_registered(::std::string_view name) {
            return ClassMap.contains(name);
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

    template <typename TInsIt>
    inline void refl_class::childs(TInsIt && it) const {
        for (::std::string_view c : _childs)
            *it++ = &refl_table::get_class(c);
    }
}

#define META_RT_REGIST static inline const RFLITE refl_iter_t meta_info_rt \
                       = RFLITE refl_table::regist<typename RFLITE_META_INFO<RFLITE_IMPL meta_null_id>::class_t>();

#define META_E_RT \
    META_E        \
    META_RT_REGIST

#define META_BE_RT(type, ...)  \
    META_BE(type, __VA_ARGS__) \
    META_RT_REGIST

RFLITE_NS {
    template <typename T>
    struct meta_traits_rt {
        static const auto &get_meta() {
            return T::meta_info_rt->second;
        }
    };
}

#else

#define META_E_RT META_E

#define META_EMPTY_RT(type, ...) META_EMPTY(type, __VA_ARGS__)

#endif
