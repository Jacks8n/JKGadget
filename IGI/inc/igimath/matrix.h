#pragma once

/// operands of templated matrix operators are desgined to be const ref,
/// in order to simplify further specialization

#include "igimath/mathutil.h"
#include "igimath/single.h"
#include "igiutilities/igiutil.h"

namespace igi {
    template <typename T, size_t Nrow, size_t Ncol>
    class matrix;

    template <typename T, size_t Nrow, size_t Ncol>
    class matrix_base;

    template <typename T>
    struct matrix_trait {
        static constexpr bool value = false;
    };

    template <typename T, size_t Nrow, size_t Ncol>
    struct matrix_trait<matrix_base<T, Nrow, Ncol>> {
        static constexpr bool value = true;

        using elem_t = T;

        static constexpr size_t row = Nrow;
        static constexpr size_t col = Ncol;
    };

    template <typename T, size_t Nrow, size_t Ncol>
    struct matrix_trait<matrix<T, Nrow, Ncol>> {
        static constexpr bool value = true;

        using elem_t = T;

        static constexpr size_t row = Nrow;
        static constexpr size_t col = Ncol;
    };

    template <typename T>
    concept is_matrix_c = matrix_trait<std::remove_cvref_t<T>>::value;

    template <typename T>
    using matrix_elem_t = typename matrix_trait<std::remove_cvref_t<T>>::elem_t;

    template <typename T>
    constexpr size_t matrix_row_v = matrix_trait<std::remove_cvref_t<T>>::row;

    template <typename T>
    constexpr size_t matrix_col_v = matrix_trait<std::remove_cvref_t<T>>::col;

    template <typename T>
    concept is_scalar_matrix_c = matrix_row_v<T> == 1 && matrix_col_v<T> == 1;

    enum class sub_matrix_type { row,
                                 column };

    template <sub_matrix_type Type, size_t Nsub, typename TMatrix>
    requires(Nsub > 0) class sub_matrix {
        static_assert(Type == sub_matrix_type::row || Type == sub_matrix_type::column);

        static constexpr size_t Nrow = matrix_row_v<TMatrix>;
        static constexpr size_t Ncol = matrix_col_v<TMatrix>;

        using element_t = matrix_elem_t<TMatrix>;

        template <typename U, size_t N>
        using matrix_t = std::conditional_t<Type == sub_matrix_type::row,
                                            matrix<U, N, Ncol>, matrix<U, Nrow, N>>;

      public:
        using sub_vector_t = matrix_t<element_t &, 1>;
        using sub_matrix_t = matrix_t<element_t &, Nsub>;
        using sub_tuple_t  = tuple_n_t<sub_vector_t, Nsub>;

        using sub_const_vector_t = matrix_t<const element_t &, 1>;
        using sub_const_matrix_t = matrix_t<const element_t &, Nsub>;
        using sub_const_tuple_t  = tuple_n_t<sub_const_vector_t, Nsub>;

      private:
        TMatrix &_mat;
        size_t _subs[Nsub];

      public:
        sub_matrix(const sub_matrix &) = default;
        sub_matrix(sub_matrix &&)      = default;

        template <typename... Ts>
        constexpr sub_matrix(TMatrix &matrix, Ts &&... is)
            : _mat(matrix), _subs { static_cast<size_t>(is)... } { }

        sub_tuple_t asTuple() {
            return asTuple<sub_vector_t>(*this);
        }

        constexpr sub_const_tuple_t asTuple() const {
            return asTuple<sub_const_vector_t>(*this);
        }

        sub_matrix_t asMatrix() {
            return asMatrix<sub_matrix_t>(*this);
        }

        constexpr sub_const_matrix_t asMatrix() const {
            return asMatrix<sub_const_matrix_t>(*this);
        }

        operator sub_tuple_t() {
            return asTuple();
        }

        constexpr operator sub_const_tuple_t() const {
            return asTuple();
        }

        operator sub_matrix_t() {
            return asMatrix();
        }

        constexpr operator sub_const_matrix_t() const {
            return asMatrix();
        }

      private:
        template <typename U, typename TSub>
        static constexpr auto asTuple(TSub &&sub) {
            return ForCE<Nsub>([&](size_t i) {
                return U([&](size_t j, size_t k) -> decltype(auto) {
                if constexpr (Type == sub_matrix_type::row)
                    return sub._mat.get(sub._subs[i], k);
                else
                    return sub._mat.get(j, sub._subs[i]); });
            });
        }

        template <typename U, typename TSub>
        static constexpr U asMatrix(TSub &&sub) {
            return U([&](size_t i, size_t j) -> decltype(auto) {
                if constexpr (Type == sub_matrix_type::row)
                    return sub._mat.get(sub._subs[i], j);
                else
                    return sub._mat.get(i, sub._subs[j]);
            });
        }
    };

    template <sub_matrix_type Type, is_matrix_c TMatrix, typename... Ts>
    constexpr auto make_sub_matrix(TMatrix &&matrix, Ts &&... is) {
        return sub_matrix<Type, sizeof...(Ts), std::remove_reference_t<TMatrix>>(matrix, std::forward<Ts>(is)...);
    }

    template <typename T, size_t Nrow, size_t Ncol>
    class matrix_base {
        static_assert(Nrow > 0 && Ncol > 0);

        template <typename Tf, size_t Nrowf, size_t Ncolf>
        friend class matrix_base;

      public:
        using row_vec_t = matrix_base<T, 1, Ncol>;
        using col_vec_t = matrix_base<T, Nrow, 1>;

        template <typename U>
        using mul_res_r_t = decltype(std::declval<T>() * std::declval<U>());
        template <typename U>
        using mul_res_l_t = decltype(std::declval<U>() * std::declval<T>());

        using element_t       = std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<std::remove_reference_t<T>>, T>;
        using const_element_t = std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<const std::remove_reference_t<T>>, const T>;
        using ref_element_t   = std::conditional_t<std::is_reference_v<T>, std::remove_reference_t<T> &, T &>;
        using cref_element_t  = std::conditional_t<std::is_reference_v<T>, const std::remove_reference_t<T> &, const T &>;

        using tuple_t = tuple_n_t<ref_element_t, Nrow * Ncol>;

      private:
        template <typename U>
        static constexpr bool is_indexable_ctor_v = std::is_invocable_v<U, size_t, size_t>;

        element_t _elem[Nrow * Ncol];

        template <typename TFrom, size_t... Is>
        constexpr explicit matrix_base(const matrix_base<TFrom, Nrow, Ncol> &o, std::index_sequence<Is...>)
            : matrix_base(o._elem[Is]...) { }

        template <typename Fn, size_t... Is>
        requires(std::is_invocable_v<Fn, size_t, size_t>) constexpr explicit matrix_base(Fn &&fn, std::index_sequence<Is...>)
            : matrix_base(static_cast<element_t>(fn(Is / Ncol, Is % Ncol))...) { }

        template <typename... Ts, size_t... Is>
        constexpr explicit matrix_base(const std::tuple<Ts...> &ts, std::index_sequence<Is...>)
            : matrix_base(std::get<Is>(ts)...) { }

        template <typename... Ts, size_t... Is>
        constexpr explicit matrix_base(const std::tuple<Ts...> &ts, std::index_sequence<Is...>, int)
            : matrix_base(std::get<(Is % Ncol) * Nrow + Is / Ncol>(ts)...) { }

      public:
        matrix_base()                    = default;
        matrix_base(const matrix_base &) = default;
        matrix_base(matrix_base &&)      = default;

        constexpr matrix_base(std::initializer_list<element_t> list) {
            std::copy(list.begin(), list.end(), _elem);
        }

        template <typename TFrom>
        constexpr explicit matrix_base(const matrix_base<TFrom, Nrow, Ncol> &o)
            : matrix_base(o, makeNIndexSeq()) { }

        template <typename... Ts>
        requires(all_convertible_v<element_t, Ts &&...>) constexpr explicit matrix_base(Ts &&... ts) : _elem { static_cast<element_t>(ts)... } { }

        template <typename Fn>
        requires(is_indexable_ctor_v<Fn &&>) constexpr explicit matrix_base(Fn &&fn)
            : matrix_base(fn, makeNIndexSeq()) { }

        template <typename... Ts>
        requires(Nrow > 1 && sizeof...(Ts) == Nrow && all_convertible_v<row_vec_t, Ts &&...>) constexpr explicit matrix_base(Ts &&... ts)
            : matrix_base(std::tuple_cat(static_cast<row_vec_t>(ts).asTuple()...), makeNIndexSeq()) { }

        template <typename... Ts>
        requires(Ncol > 1 && sizeof...(Ts) == Ncol && all_convertible_v<col_vec_t, Ts &&...> && true) constexpr explicit matrix_base(Ts &&... ts)
            : matrix_base(std::tuple_cat(static_cast<col_vec_t>(ts).asTuple()...), makeNIndexSeq(), 0) { }

        template <typename... Ts>
        constexpr matrix_base(const std::tuple<Ts...> &t)
            : matrix_base(t, std::make_index_sequence<sizeof...(Ts)>()) { }

        matrix_base &operator=(const matrix_base &) = default;
        matrix_base &operator=(matrix_base &&) = default;

        ~matrix_base() = default;

        static constexpr matrix<T, Nrow, Ncol> One(cref_element_t val = static_cast<element_t>(1)) {
            return matrix<T, Nrow, Ncol>([&](size_t, size_t) constexpr { return val; });
        }

        decltype(auto) getAll() {
            return (_elem);
        }

        decltype(auto) getAll() const {
            return (_elem);
        }

        ref_element_t get(size_t r, size_t c) {
            return _elem[r * Ncol + c];
        }

        constexpr cref_element_t get(size_t r, size_t c) const {
            return _elem[r * Ncol + c];
        }

        constexpr auto row(size_t r) {
            return make_sub_matrix<sub_matrix_type::row>(*this, r).asMatrix();
        }

        template <size_t... Is>
        constexpr auto row() {
            return make_sub_matrix<sub_matrix_type::row>(*this, Is...).asMatrix();
        }

        template <size_t... Is>
        constexpr auto row(std::index_sequence<Is...>) {
            return row<Is...>();
        }

        constexpr auto row() {
            return row(std::make_index_sequence<Nrow>());
        }

        constexpr auto row(size_t r) const {
            return make_sub_matrix<sub_matrix_type::row>(*this, r).asMatrix();
        }

        template <size_t... Is>
        constexpr auto row() const {
            return make_sub_matrix<sub_matrix_type::row>(*this, Is...).asMatrix();
        }

        template <size_t... Is>
        constexpr auto row(std::index_sequence<Is...>) const {
            return row<Is...>();
        }

        constexpr auto row() const {
            return row(std::make_index_sequence<Nrow>());
        }

        constexpr auto col(size_t c) {
            return make_sub_matrix<sub_matrix_type::column>(*this, c).asMatrix();
        }

        template <size_t... Is>
        constexpr auto col() {
            return make_sub_matrix<sub_matrix_type::column>(*this, Is...).asMatrix();
        }

        template <size_t... Is>
        constexpr auto col(std::index_sequence<Is...>) {
            return col<Is...>();
        }

        constexpr auto col() {
            return col(std::make_index_sequence<Ncol>());
        }

        constexpr auto col(size_t c) const {
            return make_sub_matrix<sub_matrix_type::column>(*this, c).asMatrix();
        }

        template <size_t... Is>
        constexpr auto col() const {
            return make_sub_matrix<sub_matrix_type::column>(*this, Is...).asMatrix();
        }

        template <size_t... Is>
        constexpr auto col(std::index_sequence<Is...>) const {
            return col<Is...>();
        }

        constexpr auto col() const {
            return col(std::make_index_sequence<Ncol>());
        }

        constexpr tuple_t asTuple() {
            return convert_impl<tuple_t>(makeNIndexSeq());
        }

        constexpr tuple_t asTuple() const {
            return convert_impl<tuple_t>(makeNIndexSeq());
        }

        void setRow(size_t r, const row_vec_t &rv) {
            for (size_t i = 0; i < Ncol; i++)
                get(r, i) = rv.get(0, i);
        }

        void setCol(size_t c, const col_vec_t &cv) {
            for (size_t i = 0; i < Nrow; i++)
                get(i, c) = cv.get(i, 0);
        }

        constexpr matrix<T, Ncol, Nrow> transpose() const {
            return matrix<T, Ncol, Nrow>([&](size_t i, size_t j) constexpr {
                return this->get(j, i);
            });
        }

        constexpr matrix<T, Nrow, Ncol> operator-() const {
            return matrix<T, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return -this->get(i, j); });
        }

        constexpr matrix<T, Nrow, Ncol> operator+(const matrix_base &r) const {
            return matrix<T, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return this->get(i, j) + r.get(i, j); });
        }

        constexpr matrix<T, Nrow, Ncol> operator-(const matrix_base &r) const {
            return matrix<T, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return this->get(i, j) - r.get(i, j); });
        }

        template <typename Tr, size_t Nrcol>
        constexpr auto operator*(const matrix_base<Tr, Ncol, Nrcol> &r) const {
            using res_elem_t = mul_res_r_t<Tr>;

            return matrix<res_elem_t, Nrow, Nrcol>([&](size_t i, size_t j) constexpr {
                res_elem_t res = this->get(i, 0) * r.get(0, j);
                for (size_t k = 1; k < Ncol; k++)
                    res += this->get(i, k) * r.get(k, j);
                return res;
            });
        }

        template <typename Tr, size_t Nrcol>
        constexpr auto transMul(const matrix_base<Tr, Nrow, Nrcol> &r) const {
            using res_elem_t = mul_res_r_t<Tr>;

            return matrix<res_elem_t, Ncol, Nrcol>([&](size_t i, size_t j) constexpr {
                res_elem_t res = this->get(0, i) * r.get(0, j);
                for (size_t k = 1; k < Nrow; k++)
                    res += this->get(k, i) * r.get(k, j);
                return res;
            });
        }

        template <typename Tr>
        constexpr matrix<T, Nrow, Ncol> operator/(Tr &&r) const {
            return matrix<T, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return this->get(i, j) / r; });
        }

        template <typename Tr>
        constexpr matrix_base<T, Nrow, Ncol> &operator+=(const matrix_base<Tr, Nrow, Ncol> &r) {
            MatrixBinaryOp(*this, r, [](ref_element_t el, cref_element_t er) { el += er; });
            return *this;
        }

        template <typename Tr>
        constexpr matrix_base<T, Nrow, Ncol> &operator-=(const matrix_base<Tr, Nrow, Ncol> &r) {
            MatrixBinaryOp(*this, r, [](ref_element_t el, cref_element_t er) { el -= er; });
            return *this;
        }

        template <typename Tr>
        constexpr matrix_base<T, Nrow, Ncol> &operator*=(const matrix_base<Tr, Nrow, Ncol> &r) {
            MatrixBinaryOp(*this, r, [](ref_element_t el, cref_element_t er) { el *= er; });
            return *this;
        }

        template <typename Tr>
        constexpr matrix_base<T, Nrow, Ncol> &operator/=(const matrix_base<Tr, Nrow, Ncol> &r) {
            MatrixBinaryOp(*this, r, [](ref_element_t el, cref_element_t er) { el /= er; });
            return *this;
        }

        template <typename U>
        requires(is_scalar_matrix_c<matrix_base>) constexpr operator U() const {
            return static_cast<U>(get(0, 0));
        }

        constexpr bool operator==(const matrix_base &r) const {
            return equal_impl(r, makeNIndexSeq());
        }

        constexpr bool operator!=(const matrix_base &r) const {
            return differ_impl(r, makeNIndexSeq());
        }

        constexpr operator tuple_t() {
            return asTuple();
        }

        constexpr operator tuple_t() const {
            return asTuple();
        }

      private:
        static constexpr auto makeNIndexSeq() {
            return std::make_index_sequence<Nrow * Ncol>();
        }

        template <typename TTo, size_t... Is>
        constexpr TTo convert_impl(std::index_sequence<Is...>) {
            return TTo(_elem[Is]...);
        }

        template <typename TTo, size_t... Is>
        constexpr TTo convert_impl(std::index_sequence<Is...>) const {
            return TTo(_elem[Is]...);
        }

        template <size_t... Is>
        constexpr bool equal_impl(const matrix_base &o, std::index_sequence<Is...>) const {
            return (static_cast<bool>(_elem[Is] == o._elem[Is]) && ...);
        }

        template <size_t... Is>
        constexpr bool differ_impl(const matrix_base &o, std::index_sequence<Is...>) const {
            return (static_cast<bool>(_elem[Is] != o._elem[Is]) || ...);
        }
    };

    namespace impl {
        template <typename T, size_t N>
        class matrix_base_sqr : public matrix_base<T, N, N> {
            using matrix_t = matrix<T, N, N>;

          public:
            using matrix_base<T, N, N>::matrix_base;
            using matrix_base<T, N, N>::get;

            static constexpr matrix_t Identity() {
                constexpr matrix_t I([](size_t i, size_t j) constexpr { return i == j ? 1 : 0; });

                return I;
            }

            template <size_t R, size_t C>
            constexpr T adjoint() const {
                return adjoint_impl<R, C>(makeNSeq(), makeNSeq());
            }

            constexpr matrix_t adjointT() const {
                return adjointT_impl(std::make_index_sequence<N * N>());
            }

            constexpr T determinant() const {
                return determinant_impl(makeNSeq(), makeNSeq());
            }

            constexpr matrix_t inverse() const {
                return adjointT() * (static_cast<T>(1) / determinant());
            }

          private:
            static constexpr auto makeNSeq() {
                return std::make_index_sequence<N>();
            }

            template <size_t R, size_t C, size_t... Rs, size_t... Cs>
            constexpr T adjoint_impl(std::index_sequence<Rs...>, std::index_sequence<Cs...>) const {
                constexpr bool neg = ((R + C) & 1) == 1;
                if constexpr (sizeof...(Rs) == 1)
                    return neg ? -get(R, C) : get(R, C);
                else {
                    constexpr auto rs = RemoveFromInts<R, Rs...>();
                    constexpr auto cs = RemoveFromInts<C, Rs...>();
                    return neg ? -determinant_impl(rs, cs) : determinant_impl(rs, cs);
                }
            }

            template <size_t... Is>
            constexpr matrix_t adjointT_impl(std::index_sequence<Is...>) const {
                return matrix_t(adjoint<Is % N, Is / N>()...);
            }

            template <size_t... Rs, size_t... Cs>
            constexpr T determinant_impl(std::index_sequence<Rs...> rs, std::index_sequence<Cs...> cs) const {
                if constexpr (sizeof...(Rs) == 1)
                    return get(Rs..., Cs...);
                else {
                    constexpr size_t r0 = GetNthInt<0, Rs...>();
                    return ((get(r0, Cs) * adjoint_impl<r0, Cs>(rs, cs)) + ...);
                }
            }
        };
    }  // namespace impl

    template <is_matrix_c TL, is_matrix_c TR, typename TFn>
    static constexpr void MatrixBinaryOp(TL &&l, TR &&r, TFn &&fn) {
        constexpr size_t Nrow = matrix_row_v<TL>;
        static_assert(Nrow == matrix_row_v<TR>);

        constexpr size_t Ncol = matrix_col_v<TL>;
        static_assert(Ncol == matrix_col_v<TR>);

        for (size_t i = 0; i < Nrow; i++)
            for (size_t j = 0; j < Ncol; j++)
                fn(l.get(i, j), r.get(i, j));
    }

    template <typename T, size_t Nrow, size_t Ncol, typename TR>
    requires(!is_matrix_c<TR>) constexpr auto operator*(const matrix_base<T, Nrow, Ncol> &l, const TR &r) {
        using res_t = typename matrix_base<T, Nrow, Ncol>::template mul_res_r_t<TR>;

        return matrix<res_t, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return l.get(i, j) * r; });
    }

    template <typename TL, typename T, size_t Nrow, size_t Ncol>
    requires(!is_matrix_c<TL>) constexpr auto operator*(const TL &l, const matrix_base<T, Nrow, Ncol> &r) {
        using res_t = typename matrix_base<T, Nrow, Ncol>::template mul_res_l_t<TL>;

        return matrix<res_t, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return l * r.get(i, j); });
    }

    template <typename To, size_t Nrow, size_t Ncol, typename T>
    To &operator<<(To &out, const matrix_base<T, Nrow, Ncol> &m) {
        out << '{';
        for (size_t i = 0; i < Nrow - 1; i++) {
            out << '{';
            for (size_t j = 0; j < Ncol - 1; j++) out << m.get(i, j) << ' ';
            out << m.get(i, Ncol - 1) << "} ";
        }
        out << '{';
        for (size_t j = 0; j < Ncol - 1; j++) out << m.get(Nrow - 1, j) << ' ';
        out << m.get(Nrow - 1, Ncol - 1) << "}}";
        return out;
    }

    template <typename T, size_t Nrow, size_t Ncol>
    void swap(matrix_base<T, Nrow, Ncol> &l, matrix_base<T, Nrow, Ncol> &r) {
        std::swap(l.getAll(), r.getAll());
    }

    template <typename T, size_t Nrow, size_t Ncol>
    class matrix : public matrix_base<T, Nrow, Ncol> {
        using matrix_base<T, Nrow, Ncol>::matrix_base;

      public:
        matrix() = default;
    };

    template <typename T, size_t N>
    class matrix<T, N, N> : public impl::matrix_base_sqr<T, N> {
        using impl::matrix_base_sqr<T, N>::matrix_base_sqr;

      public:
        matrix() = default;
    };

    template <size_t Nrow, size_t Ncol>
    using matrixi = matrix<int, Nrow, Ncol>;
    template <size_t Nrow, size_t Ncol>
    using matrixf = matrix<single, Nrow, Ncol>;

    template <typename T>
    using mat2    = matrix<T, 2, 2>;
    using mat2x2i = mat2<int>;
    using mat2x2f = matrixf<2, 2>;

    template <typename T>
    using mat3    = matrix<T, 3, 3>;
    using mat3x3i = mat3<int>;
    using mat3x3f = matrixf<3, 3>;

    template <typename T>
    using mat4    = matrix<T, 4, 4>;
    using mat4x4i = mat4<int>;
    using mat4x4f = matrixf<4, 4>;
}  // namespace igi
