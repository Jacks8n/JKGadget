#pragma once

#include <array>
#include "igiutil.h"
#include "mathutil.h"
#include "numconfig.h"

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
    constexpr bool is_matrix_v = matrix_trait<T>::value;

    template <typename T>
    using matrix_elem_t = typename matrix_trait<T>::elem_t;

    template <typename T, size_t Nrow, size_t Ncol>
    class matrix_base {
        template <typename Tf, size_t Nrowf, size_t Ncolf>
        friend class matrix_base;

        std::array<T, Nrow * Ncol> _elem;

        template <typename FromT, size_t... Is>
        constexpr explicit matrix_base(const matrix_base<FromT, Nrow, Ncol> &o, std::index_sequence<Is...>)
            : matrix_base(o._elem[Is]...) { }

        template <typename Fn, size_t... Is, std::enable_if_t<std::is_invocable_v<Fn, size_t, size_t>, int> = 0>
        constexpr explicit matrix_base(Fn &&fn, std::index_sequence<Is...>)
            : matrix_base(fn(Is / Ncol, Is % Ncol)...) { }

        template <typename... Ts, typename TFn, size_t... Is>
        constexpr explicit matrix_base(std::tuple<Ts...> ts, TFn &&fn, std::index_sequence<Is...>)
            : matrix_base(std::get<fn(Is)>(ts)...) { }

        template <typename... Ts, size_t... Is>
        constexpr explicit matrix_base(std::tuple<Ts...> ts, std::index_sequence<Is...>, int)
            : matrix_base(std::get<(Is % Ncol) * Nrow + Is / Ncol>(ts)...) { }

      public:
        using row_vec_t = matrix_base<T, 1, Ncol>;
        using col_vec_t = matrix_base<T, Nrow, 1>;

        matrix_base()                    = default;
        matrix_base(const matrix_base &) = default;
        matrix_base(matrix_base &&)      = default;
        matrix_base(std::initializer_list<T> list) {
            std::copy(list.begin(), list.end(), _elem);
        }

        template <typename... Ts, std::enable_if_t<all_convertible_v<T, Ts...>, int> = 0>
        constexpr explicit matrix_base(Ts &&... ts) : _elem { static_cast<T>(ts)... } { }

        template <typename... Ts, std::enable_if_t<all_convertible_v<row_vec_t, Ts...>, int> = 0>
        constexpr explicit matrix_base(Ts &&... ts)
            : matrix_base(std::tuple_cat(ts.asTuple()...), makeNCompSeq()) { }

        template <typename... Ts, std::enable_if_t<all_convertible_v<col_vec_t, Ts...>, int> = 0>
        constexpr explicit matrix_base(Ts &&... ts)
            : matrix_base(std::tuple_cat(ts.asTuple()...), makeNCompSeq(), 0) { }

        template <typename FromT>
        constexpr explicit matrix_base(const matrix_base<FromT, Nrow, Ncol> &o)
            : matrix_base(o, makeNCompSeq()) { }

        template <typename Fn, std::enable_if_t<std::is_invocable_v<Fn, size_t, size_t>, int> = 0>
        constexpr explicit matrix_base(Fn &&fn)
            : matrix_base(fn, makeNCompSeq()) { }

        matrix_base &operator=(const matrix_base &) = default;
        matrix_base &operator=(matrix_base &&) = default;

        ~matrix_base() = default;

        static constexpr matrix<T, Nrow, Ncol> One(const T &val = 1) {
            return matrix<T, Nrow, Ncol>([&](size_t, size_t) constexpr { return val; });
        }

        std::array<T, Nrow * Ncol> &getAll() {
            return (_elem);
        }

        const std::array<T, Nrow * Ncol> &getAll() const {
            return (_elem);
        }

        T &get(size_t r, size_t c) {
            return _elem[r * Ncol + c];
        }

        constexpr const T &get(size_t r, size_t c) const {
            return _elem[r * Ncol + c];
        }

        constexpr matrix<T, 1, Ncol> row(size_t r) const {
            return row_impl(r, makeColSeq());
        }

        constexpr auto rows() const {
            return ForCE<Nrow>(
                [](size_t i, const matrix_base &m) constexpr { return m.row(i); }, *this);
        }

        constexpr matrix<T, Nrow, 1> col(size_t c) const {
            return col_impl(c, makeRowSeq());
        }

        constexpr auto cols() const {
            return ForCE<Ncol>(
                [](size_t i, const matrix_base &m) constexpr { return m.col(i); }, *this);
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
            return matrix_base<T, Ncol, Nrow>([&](size_t i, size_t j) constexpr {
                return this->get(j, i);
            });
        }

        template <typename Tr,
                  std::enable_if_t<!is_matrix_v<std::remove_reference_t<Tr>>, int> = 0>
        constexpr matrix<T, Nrow, Ncol> operator*(Tr &&r) const {
            return matrix<T, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return this->get(i, j) * r; });
        }

        template <typename Tr, std::enable_if_t<is_matrix_v<std::remove_reference_t<Tr>>, int> = 0>
        constexpr auto operator*(Tr &&r) const {
            using r_elem_t   = std::remove_reference_t<decltype(r.get(0, 0))>;
            using mul_vec_t  = matrix_base<r_elem_t, Ncol, 1>;
            using res_elem_t = std::remove_reference_t<decltype(get(0, 0) * r.get(0, 0))>;

            constexpr size_t Nrcol = matrix_trait<std::remove_reference_t<Tr>>::col;

            constexpr auto dot = [](size_t i, const mul_vec_t &cv, const matrix_base &m) constexpr {
                return std::apply(
                    [](auto &&... ms) constexpr {
                        return (ms + ...);
                    },
                    ForCE<Ncol>([&](size_t j) constexpr {
                        return m.get(i, j) * cv.get(j, 0);
                    }));
            };
            constexpr auto mulCol = [&](const mul_vec_t &cv, const matrix_base &m, auto &&dt) constexpr {
                return std::apply(
                    [](auto &&... es) constexpr {
                        return matrix_base<res_elem_t, Nrow, 1>(std::forward<decltype(es)>(es)...);
                    },
                    ForCE<Nrow>(
                        [&](size_t i, const matrix_base &m, auto &&dt) constexpr {
                            return dt(i, cv, m);
                        },
                        m, dt));
            };
            return std::apply(
                [](auto &&... cols) constexpr {
                    return matrix<res_elem_t, Nrow, Nrcol>(std::forward<decltype(cols)>(cols)...);
                },
                ForCE<Nrcol>(
                    [&](size_t i, const matrix_base &m, auto &&dt) constexpr {
                        return mulCol(r.col(i), m, dt);
                    },
                    *this, dot));
        }

        template <typename Tr, size_t Nrcol>
        constexpr auto transMul(const matrix_base<Tr, Nrow, Nrcol> &r) const {
            using mul_vec_t  = matrix_base<Tr, Nrow, 1>;
            using res_elem_t = std::remove_reference_t<decltype(get(0, 0) * r.get(0, 0))>;

            constexpr auto dot = [](size_t i, const mul_vec_t &cv, const matrix_base &m) constexpr {
                return std::apply(
                    [](auto &&... ms) constexpr {
                        return (ms + ...);
                    },
                    ForCE<Nrow>([&](size_t j) constexpr {
                        return m.get(j, i) * cv.get(j, 0);
                    }));
            };
            constexpr auto mulCol = [&](const mul_vec_t &cv, const matrix_base &m, auto &&dt) constexpr {
                return std::apply(
                    [](auto &&... es) constexpr {
                        return matrix_base<res_elem_t, Ncol, 1>(std::forward<decltype(es)>(es)...);
                    },
                    ForCE<Ncol>(
                        [&](size_t i, const matrix_base &m, auto &&dt) constexpr {
                            return dt(i, cv, m);
                        },
                        m, dt));
            };
            return std::apply(
                [](auto &&... cols) constexpr {
                    return matrix<res_elem_t, Ncol, Nrcol>(std::forward<decltype(cols)>(cols)...);
                },
                ForCE<Nrcol>(
                    [&](size_t i, const matrix_base &m, auto &&dt) constexpr {
                        return mulCol(r.col(i), m, dt);
                    },
                    *this, dot));
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

        template <typename Tr>
        constexpr matrix<T, Nrow, Ncol> operator/(Tr &&r) const {
            return matrix<T, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return this->get(i, j) / r; });
        }

        constexpr bool operator==(const matrix_base &r) const {
            return equal_impl(r, makeNCompSeq());
        }

        constexpr bool operator!=(const matrix_base &r) const {
            return differ_impl(r, makeNCompSeq());
        }

        constexpr operator matrix<T, Nrow, Ncol>() const {
            return convert_impl<matrix<T, Nrow, Ncol>>(makeNCompSeq());
        }

      private:
        constexpr auto asTuple() const {
            constexpr auto seq = makeNCompSeq();
            return convert_impl<decltype(MakeTupleN<T>(seq))>(seq);
        }

        static constexpr auto makeRowSeq() {
            return std::make_index_sequence<Nrow>();
        }

        static constexpr auto makeColSeq() {
            return std::make_index_sequence<Ncol>();
        }

        static constexpr auto makeNCompSeq() {
            return std::make_index_sequence<Nrow * Ncol>();
        }

        template <size_t... Cs>
        constexpr matrix<T, 1, Ncol> row_impl(size_t r, std::index_sequence<Cs...>) const {
            return matrix<T, 1, Ncol>(get(r, Cs)...);
        }

        template <size_t... Rs>
        constexpr matrix<T, Nrow, 1> col_impl(size_t c, std::index_sequence<Rs...>) const {
            return matrix<T, Nrow, 1>(get(Rs, c)...);
        }

        template <typename TTo, size_t... Is>
        constexpr TTo convert_impl(std::index_sequence<Is...>) const {
            return TTo(_elem[Is]...);
        }

        template <size_t... Is>
        constexpr bool equal_impl(const matrix_base &o, std::index_sequence<Is...>) const {
            return ((_elem[Is] == o._elem[Is]) && ... && true);
        }

        template <size_t... Is>
        constexpr bool differ_impl(const matrix_base &o, std::index_sequence<Is...>) const {
            return ((_elem[Is] != o._elem[Is]) || ... || false);
        }
    };

    template <typename T>
    class matrix_base<T, 1, 1> {
        T _elem;

      public:
        matrix_base() = default;
        constexpr explicit matrix_base(const T &e) : _elem(e) { }

        T &get(size_t, size_t) {
            return _elem;
        }

        const T &get(size_t, size_t) const {
            return _elem;
        }

        constexpr operator T() const {
            return _elem;
        }
    };

    template <typename T, size_t N>
    class matrix_base_sqr : public matrix_base<T, N, N> {
        using matrix_t = matrix<T, N, N>;

      public:
        using matrix_base<T, N, N>::matrix_base;
        using matrix_base<T, N, N>::get;

        matrix_base_sqr() = default;

        static constexpr matrix_t Identity() {
            return matrix<T, N, N>([](size_t i, size_t j) constexpr { return i == j ? 1 : 0; });
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

    template <typename Tl, typename T, size_t Nrow, size_t Ncol,
              std::enable_if_t<std::is_convertible_v<std::remove_reference_t<Tl>, T>, int> = 0>
    constexpr auto operator*(Tl &&l, const matrix_base<T, Nrow, Ncol> &r) {
        using res_t = std::remove_reference_t<decltype(l * r.get(0, 0))>;
        return matrix<res_t, Nrow, Ncol>([&](size_t i, size_t j) constexpr { return l * r.get(i, j); });
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

    template <typename T>
    class matrix<T, 1, 1> : public matrix_base_sqr<T, 1> {
        using matrix_base_sqr<T, 1>::matrix_base_sqr;

      public:
        matrix() = default;
    };

    template <typename T, size_t N>
    class matrix<T, N, N> : public matrix_base_sqr<T, N> {
        using matrix_base_sqr<T, N>::matrix_base_sqr;

      public:
        matrix() = default;
    };

    template <size_t Nrow, size_t Ncol>
    using matrixf = matrix<single, Nrow, Ncol>;
    template <size_t Nrow, size_t Ncol>
    using matrixi = matrix<int, Nrow, Ncol>;

    using mat2x2f = matrix<single, 2, 2>;
    using mat2x2i = matrix<int, 2, 2>;

    using mat3x3f = matrix<single, 3, 3>;
    using mat3x3i = matrix<int, 3, 3>;

    using mat4x4i = matrix<int, 4, 4>;

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
}  // namespace igi
