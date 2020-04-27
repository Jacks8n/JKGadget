#pragma once

#include "igiutil.h"
#include "mathutil.h"
#include "numconfig.h"

namespace igi {
    template <size_t Nrow, size_t Ncol, typename T>
    class matrix;

    template <size_t Nrow, size_t Ncol, typename T>
    class matrix_base {
        T _elem[Nrow * Ncol];

    public:
        using row_vec_t = matrix_base<1, Ncol, T>;
        using col_vec_t = matrix_base<Nrow, 1, T>;
        using mul_vec_t = matrix_base<Ncol, 1, T>;

        matrix_base() = default;
        matrix_base(const matrix_base&) = default;
        matrix_base(matrix_base&&) = default;
        matrix_base(std::initializer_list<T> list) {
            std::copy(list.begin(), list.end(), _elem);
        }

        template <
            typename... Ts,
            typename = typename std::enable_if<
            std::is_convertible<typename first_t<Ts...>::type, T>::value>::type>
            constexpr matrix_base(Ts&&... ts) : _elem{ static_cast<T>(ts)... } {}

        template <typename FromT,
            typename = typename std::enable_if<
            std::is_convertible<FromT, T>::value>::type>
            constexpr explicit matrix_base(const matrix_base<Nrow, Ncol, FromT>& o) {
            foreach([&](T& e, size_t i, size_t j) { e = static_cast<FromT>(o.get(i, j)); });
        }

        matrix_base& operator=(const matrix_base&) = default;
        matrix_base& operator=(matrix_base&&) = default;

        ~matrix_base() = default;

        static constexpr matrix_base one(T val = 1) {
            return one_impl(val, std::make_index_sequence<Nrow* Ncol>());
        }

        T& get(size_t r, size_t c) { return _elem[r * Ncol + c]; }

        constexpr const T& get(size_t r, size_t c) const {
            return _elem[r * Ncol + c];
        }

        row_vec_t getRow(size_t r) const {
            row_vec_t res;
            for (size_t i = 0; i < Ncol; i++)
                res.get(0, i) = get(r, i);
            return res;
        }

        col_vec_t getCol(size_t c) const {
            col_vec_t res;
            for (size_t i = 0; i < Nrow; i++)
                res.get(i, 0) = get(i, c);
            return res;
        }

        void setRow(size_t r, const row_vec_t& rv) {
            for (size_t i = 0; i < Ncol; i++)
                get(r, i) = rv.get(0, i);
        }

        void setCol(size_t c, const col_vec_t& cv) {
            for (size_t i = 0; i < Nrow; i++)
                get(i, c) = cv.get(i, 0);
        }

        template <typename F>
        auto foreach(F f) -> decltype(f((new T)[0]), void()) {
            for (size_t i = 0; i < Nrow; i++)
                for (size_t j = 0; j < Ncol; j++)
                    f(get(i, j));
        }

        template <typename F>
        auto foreach(F f) -> decltype(f((new T)[0], size_t(), size_t()), void()) {
            for (size_t i = 0; i < Nrow; i++)
                for (size_t j = 0; j < Ncol; j++)
                    f(get(i, j), i, j);
        }

        matrix_base<Ncol, Nrow, T> transpose() const {
            matrix_base<Ncol, Nrow, T> res;
            for (size_t i = 0; i < Ncol; i++)
                for (size_t j = 0; j < Nrow; j++)
                    res.get(i, j) = get(j, i);
            return res;
        }

        constexpr operator matrix<Nrow, Ncol, T>() const {
            return convert_impl(std::make_index_sequence<Nrow* Ncol>());
        }

    private:
        template <size_t... Is>
        static constexpr matrix_base one_impl(T val, std::index_sequence<Is...>) {
            return matrix_base((Is, val)...);
        }

        template <size_t... Is>
        constexpr matrix<Nrow, Ncol, T>
            convert_impl(std::index_sequence<Is...>) const {
            return matrix<Nrow, Ncol, T>(_elem[Is]...);
        }
    };

#pragma region matrix

    template <size_t Nrow, size_t Ncol, typename T>
    class matrix;

    template <typename T>
    class matrix<1, 1, T> : public matrix_base<1, 1, T> {
    public:
        matrix() = default;
        matrix(const matrix&) = default;
        matrix(matrix&&) = default;
        matrix(T val) : matrix_base<1, 1, T>(val) {}

        matrix& operator=(const matrix&) = default;
        matrix& operator=(matrix&&) = default;

        ~matrix() = default;

        operator T() { return matrix_base<1, 1, T>::get(0, 0); }
    };

    template <size_t Nrow, size_t Ncol, typename T>
    class matrix : public matrix_base<Nrow, Ncol, T> {
        using matrix_base<Nrow, Ncol, T>::matrix_base;

    public:
        matrix() = default;
        matrix(const matrix&) = default;
        matrix(matrix&&) = default;

        matrix& operator=(const matrix&) = default;
        matrix& operator=(matrix&&) = default;

        ~matrix() = default;

        template <size_t Nrcol>
        matrix_base<Nrow, Nrcol, T> operator*(
            const matrix_base<Ncol, Nrcol, T>& r) const {
            matrix_base<Nrow, Nrcol, T> res;
            mul_vec_t col;

            for (size_t i = 0; i < Nrcol; i++) {
                col = r.getCol(i);
                for (size_t j = 0; j < Nrow; j++) {
                    for (size_t k = 0; k < Ncol; k++)
                        res.get(j, i) += col.get(k, 0) * get(j, k);
                }
            }

            return res;
        }

        template <size_t Nrcol>
        matrix_base<Nrow, Nrcol, T>& operator*=(const matrix_base<Ncol, Nrcol, T>& r) {
            *this = *this * r;
            return *this;
        }
    };

    template <size_t N, typename T>
    inline matrix<N, N, T> Identity() {
        matrix<N, N, T> res;
        res.foreach([](T& e, size_t i, size_t j) { e = static_cast<T>(i == j ? 1 : 0); });
        return res;
    }

    template <size_t N, typename T>
    inline T Determinant(const matrix<N, N, T>& m) {
        constexpr auto seq = std::make_index_sequence<N>();
        return _determinant_impl(m, seq, seq);
    }

    template <size_t R, size_t C, size_t N, typename T>
    inline T Cofactor(const matrix<N, N, T>& m) {
        constexpr auto seq = std::make_index_sequence<N>();
        return _cofactor_impl<R, C>(m, seq, seq);
    }

    template <size_t N, typename T>
    inline matrix<N, N, T> AdjointTranspose(const matrix<N, N, T>& m) {
        constexpr auto seq = std::make_index_sequence<Nrow>();
        matrix<N, N, T> res;
        _adjointTranspose_impl(m, res, seq, seq);
        return res;
    }

    template <size_t N, typename T>
    inline matrix<N, N, T> Inverse(const matrix<N, N, T>& m) {
        return adjointTranspose(m) * (static_cast<T>(1) / determinant(m));
    }

#pragma endregion

#pragma region Implement

    template <size_t R, size_t C, size_t... Rs, size_t... Cs, size_t N, typename T>
    inline T _cofactor_impl(const matrix<N, N, T>& m, std::index_sequence<Rs...> rs, std::index_sequence<Cs...> cs) {
        T det = _determinant_impl(m, removeNthInt<R>(rs), removeNthInt<C>(cs));
        return (R + C) % 2 ? -det : det;
    }

    template <size_t... Rs, size_t... Cs, size_t N, typename T>
    inline T _determinant_impl(const matrix<N, N, T>& m, std::index_sequence<Rs...> rs, std::index_sequence<Cs...> cs) {
        if constexpr (sizeof...(Rs) == 1)
            return m.get(Rs..., Cs...);
        else
            return _determinant_impl(m, rs, cs, std::make_index_sequence<sizeof...(Rs)>());
    }

    template <size_t... Rs, size_t... Cs, size_t... Is, size_t N, typename T>
    inline T _determinant_impl(const matrix<N, N, T>& m, std::index_sequence<Rs...> rs, std::index_sequence<Cs...> cs, std::index_sequence<Is...>) {
        return ((m.get(getFirstInt(rs), getNthInt<Is>(cs)) * _cofactor_impl<0, Is>(m, rs, cs)) + ...);
    }

    template <size_t R0, size_t... Rs, size_t... Cs, size_t N, typename T>
    inline void _adjointTranspose_impl(const matrix<N, N, T>& m, matrix<N, N, T>& res, std::index_sequence<Cs...> cs, std::index_sequence<R0, Rs...>) {
        _adjointTranspose_impl<R0>(m, res, cs);
        if constexpr (sizeof...(Rs) != 0)
            _adjointTranspose_impl(m, res, cs, std::index_sequence<Rs...>());
    }

    template <size_t R0, size_t C0, size_t... Cs, size_t N, typename T>
    inline void _adjointTranspose_impl(const matrix<N, N, T>& m, matrix<N, N, T>& res, std::index_sequence<C0, Cs...>) {
        res.get(R0, C0) = cofactor<C0, R0>();
        if constexpr (sizeof...(Cs) != 0)
            _adjointTranspose_impl<R0>(m, res, std::index_sequence<Cs...>());
    }

#pragma endregion

    template <size_t Nrow, size_t Ncol>
    using matrixf = matrix<Nrow, Ncol, single>;
    template <size_t Nrow, size_t Ncol>
    using matrixi = matrix<Nrow, Ncol, int>;

    template <size_t N, typename T>
    using matrixSquare = matrix<N, N, T>;

    using mat2x2f = matrixSquare<2, single>;
    using mat2x2i = matrixSquare<2, int>;

    using mat3x3f = matrixSquare<3, single>;
    using mat3x3i = matrixSquare<3, int>;

    using mat4x4i = matrixSquare<4, int>;

#pragma region operator

    template <size_t Nrow, size_t Ncol, typename T>
    matrix<Nrow, Ncol, T> operator-(const matrix<Nrow, Ncol, T>& m) {
        matrix<Nrow, Ncol, T> res = m;
        res.foreach([](T& e) { e = -e; });
        return res;
    }

    template <size_t Nrow, size_t Ncol, typename T>
    matrix<Nrow, Ncol, T> operator+(const matrix<Nrow, Ncol, T>& l,
                                    const matrix<Nrow, Ncol, T>& r) {
        matrix<Nrow, Ncol, T> res = l;
        res.foreach([&](T& e, size_t i, size_t j) { e += r.get(i, j); });
        return res;
    }

    template <size_t Nrow, size_t Ncol, typename T>
    matrix<Nrow, Ncol, T> operator-(const matrix<Nrow, Ncol, T>& l,
                                    const matrix<Nrow, Ncol, T>& r) {
        matrix<Nrow, Ncol, T> res = l;
        res.foreach([&](T& e, size_t i, size_t j) { e -= r.get(i, j); });
        return res;
    }

    template <size_t Nrow, size_t Ncol, typename T>
    matrix<Nrow, Ncol, T> operator*(const matrix<Nrow, Ncol, T>& l, T r) {
        matrix<Nrow, Ncol, T> res = l;
        res.foreach([&](T& e, size_t i, size_t j) { e *= r; });
        return res;
    }

    template <size_t Nrow, size_t Ncol, typename T>
    matrix<Nrow, Ncol, T> operator/(const matrix<Nrow, Ncol, T>& l, T r) {
        matrix<Nrow, Ncol, T> res = l;
        res.foreach([&](T& e, size_t i, size_t j) { e /= r; });
        return res;
    }

    template <typename To, size_t Nrow, size_t Ncol, typename T>
    To& operator<<(To& out, const matrix<Nrow, Ncol, T>& m) {
        out << '{';
        for (size_t i = 0; i < Nrow - 1; i++) {
            out << '{';
            for (size_t j = 0; j < Ncol - 1; j++)
                out << m.get(i, j) << ' ';
            out << m.get(i, Ncol - 1) << "} ";
        }
        out << '{';
        for (size_t j = 0; j < Ncol - 1; j++)
            out << m.get(Nrow - 1, j) << ' ';
        out << m.get(Nrow - 1, Ncol - 1) << "}}";
        return out;
    }

    template <size_t Nrow, size_t Ncol, size_t Nrcol, typename T>
    matrix<Ncol, Nrcol, T> TransposeMul(const matrix<Nrow, Ncol, T>& l,
                                        const matrix<Nrow, Nrcol, T>& r) {
        matrix<Ncol, Nrcol, T> res = matrix<Ncol, Nrcol, T>::one(0);
        matrix<Nrow, 1, T> col;

        for (size_t i = 0; i < Nrcol; i++) {
            col = r.getCol(i);
            for (size_t j = 0; j < Ncol; j++) {
                for (size_t k = 0; k < Nrow; k++)
                    res.get(j, i) += col.get(k, 0) * l.get(j, k);
            }
        }

        return res;
    }

    template <size_t Nrow, size_t Ncol, typename T>
    void swap(matrix<Nrow, Ncol, T>& l, matrix<Nrow, Ncol, T>& r) {
        l.foreach([&](T& e, size_t i, size_t j) { e = r.get(i, j); });
    }

#pragma endregion
}  // namespace igi
