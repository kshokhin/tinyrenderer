#pragma once

#include<iostream>
#include<array>
#include<cstdint>
#include<cmath>
#include<vector>
#include<numeric>

namespace sk
{
template<typename T, size_t N>
class matrix_line_proxy
{
public:
    explicit matrix_line_proxy(T* data) : m_data(data) {}
    T& operator[](size_t idx) { if (idx >= N) throw std::out_of_range(""); return m_data[idx]; }
private:
    T* m_data = nullptr;
};

template<typename T, size_t N>
class const_matrix_line_proxy
{
public:
    explicit const_matrix_line_proxy(const T* data) : m_data(data) {}
    const T& operator[](size_t idx) const { if (idx >= N) throw std::out_of_range(""); return m_data[idx]; }
private:
    const T* m_data = nullptr;
};

template<typename T, size_t N, size_t M>
class matrix
{
public:
    using value_type = T;

    matrix() : m_data{ 0 } {};
    explicit matrix(const T *data)
    {
        for (size_t i = 0; i < N; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                m_data[i*M + j] = data[i*M + j];
            }
        }
        //memcpy(m_data.data(), data, N*M);
    }
    explicit matrix(const std::array<T, N*M>& data) { memcpy(m_data.data(), data.data(), N*M); }
    explicit matrix(const std::vector<T>& data)
    {
        if (data.size() > N*M) throw std::bad_alloc();
        memcpy(m_data.data(), data.data(), N*M);
    }

    constexpr static size_t row_cnt = N;
    constexpr static size_t column_cnt = M;

    matrix_line_proxy<T, M> operator[](size_t idx)
    {
        if (idx >= N) throw std::out_of_range("");
        return matrix_line_proxy<T, M>(&m_data[M*idx]);
    }

    const_matrix_line_proxy<T, M> operator[](size_t idx) const
    {
        if (idx >= N) throw std::out_of_range("");
        return const_matrix_line_proxy<T, M>(&m_data[M*idx]);
    }

    void set_row(const std::vector<T>& /*r*/, size_t /*row_idx*/);
    void set_matrix_column(const std::vector<T>& /*c*/, size_t /*col_idx*/);
    matrix<T, M, N> transpose();
private:
    std::array<T, N*M> m_data;
};

template<typename T, size_t N, size_t M>
void matrix<T, N, M>::set_row(const std::vector<T>& r, size_t row_idx)
{
    if (r.size() != M || row_idx >= N) throw std::out_of_range("");

    //memcpy(m_data.data() + row_idx*M, r.data(), r.size());
    for (size_t i = 0; i < M; ++i)
    {
        m_data[row_idx*M + i] = r[i];
    }
}

template<typename T, size_t N, size_t M>
void matrix<T, N, M>::set_matrix_column(const std::vector<T>& c, size_t col_idx)
{
    if (c.size() != N || col_idx >= M) throw std::out_of_range("");

    for (size_t i = 0; i < N; ++i)
    {
        m_data[i*M + col_idx] = c[i];
    }
}

template<typename T, size_t N, size_t M>
matrix<T, M, N> matrix<T, N, M>::transpose()
{
    matrix<T, M, N> res;

    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j < M; ++j)
        {
            res.m_data[j*N + i] = m_data[i*M + j];
        }
    }

    return res;
}

using matrix4x4f = matrix<float, 4, 4>;
using matrix3x3f = matrix<float, 3, 3>;
using matrix1x3f = matrix<float, 1, 3>;
using matrix1x4f = matrix<float, 1, 4>;
using matrix3x1f = matrix<float, 3, 1>;
using matrix4x1f = matrix<float, 4, 1>;
using matrix4x4i = matrix<int, 4, 4>;
using matrix3x3i = matrix<int, 3, 3>;
using matrix1x3i = matrix<int, 1, 3>;
using matrix1x4i = matrix<int, 1, 4>;
using matrix3x1i = matrix<int, 3, 1>;
using matrix4x1i = matrix<int, 4, 1>;

template<typename T, size_t N, size_t M, size_t P>
matrix<T, N, P> mul(const matrix<T, N, M>& m1, const matrix<T, M, P>& m2)
{
    matrix<T, N, P> res;

    for (size_t i = 0; i < N; ++i)
    {
        const auto& m1_row = m1[i];
        for (size_t j = 0; j < P; ++j)
        {
            for (size_t l = 0; l < M; ++l)
            {
                res[i][j] += m1_row[l] * m2[l][j];
            }
        }
    }

    return res;
}

template<typename T, size_t N, size_t M>
std::ostream& operator<<(std::ostream& out, const matrix<T, N, M>& m)
{
    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j < M; ++j)
        {
            out << m[i][j] << " ";
        }
        out << "\n";
    }

    return out;
}

template<typename T, size_t N>
matrix<T, N, N> identity_matrix()
{
    matrix<T, N, N> m;

    for (size_t i = 0; i < N; ++i)
    {
        m[i][i] = 1;
    }

    return m;
}

template<typename T, size_t ...indexes>
class vec_impl
{
public:
    constexpr vec_impl() { (m_data[indexes] = ... = 0); }
    constexpr vec_impl(const vec_impl& rhs) { ((m_data[indexes] = rhs.m_data[indexes]), ...); }
    template <typename...Ts>
    constexpr vec_impl(Ts ...vals) { static_assert(sizeof...(vals) == sizeof...(indexes)); ((m_data[indexes] = vals), ...); }
    constexpr vec_impl(const matrix<T, sizeof...(indexes), 1>& m)
    {
        ((m_data[indexes] = m[indexes][0]), ...);
    }

    constexpr T& operator[](size_t i) { return m_data[i]; }
    constexpr const T& operator[](size_t i) const { return m_data[i]; }
    constexpr T len() const { return std::sqrt(dot(*this, *this)); }
    constexpr void norm() { auto l = len(); ((m_data[indexes] /= l), ...); };
    constexpr vec_impl& operator=(const matrix<T, sizeof...(indexes), 1>& m)
    {
        ((m_data[indexes] = m[indexes][0]), ...);
    }

    constexpr matrix<T, sizeof...(indexes), 1> as_matrix_col() const
    {
        return matrix<T, sizeof...(indexes), 1>(m_data.data());
    }

    constexpr matrix<T, 1, sizeof...(indexes)> as_matrix_row() const
    {
        return matrix<T, 1, sizeof...(indexes)>(m_data.data());
    }
private:
    std::array<T, sizeof...(indexes)> m_data;
};

template<typename T, size_t ...indexes>
constexpr T dot(const vec_impl<T, indexes...>& lhs, const vec_impl<T, indexes...>& rhs)
{
    return ((lhs[indexes] * rhs[indexes]) + ...);
}

template<typename T, size_t ...indexes>
constexpr vec_impl<T, indexes...> operator+(const vec_impl<T, indexes...>& lhs, const vec_impl<T, indexes...>& rhs)
{
    vec_impl<T, indexes...> res;

    ((res[indexes] = (lhs[indexes] + rhs[indexes])), ...);

    return res;
}

template<typename T, size_t ...indexes>
constexpr vec_impl<T, indexes...> operator-(const vec_impl<T, indexes...>& lhs, const vec_impl<T, indexes...>& rhs)
{
    vec_impl<T, indexes...> res;

    ((res[indexes] = (lhs[indexes] - rhs[indexes])), ...);

    return res;
}

template<typename T, size_t ...indexes>
constexpr vec_impl<T, indexes...> average(const std::vector<vec_impl<T, indexes...>>& vs)
{
    auto res = std::accumulate(vs.begin(), vs.end(), vec_impl<T, indexes...>{});

    res = res / vs.size();

    return res;
}

template<typename TFirst, typename ...TOthers>
constexpr TFirst average(TFirst first, TOthers&& ...others)
{
    return ((first + others) + ...) / (sizeof...(others) + 1);
}

template<typename T, typename U, size_t ...indexes>
constexpr vec_impl<T, indexes...> operator*(const vec_impl<T, indexes...>& lhs, U v)
{
    vec_impl<T, indexes...> res;

    ((res[indexes] = (lhs[indexes] * v)), ...);

    return res;
}

template<typename T, typename U, size_t ...indexes>
constexpr vec_impl<T, indexes...> operator/(const vec_impl<T, indexes...>& lhs, U v)
{
    vec_impl<T, indexes...> res;

    ((res[indexes] = (lhs[indexes] / v)), ...);

    return res;
}

template<typename T>
constexpr vec_impl<T, 0, 1, 2> cross(const vec_impl<T, 0, 1, 2>& lhs, const vec_impl<T, 0, 1, 2>& rhs)
{
    vec_impl<T, 0, 1, 2> res;

    res[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    res[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    res[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];

    //    std::cout << lhs << rhs << res;

    return res;
}

template<typename T, size_t ...indexes>
std::ostream& operator<<(std::ostream& out, const vec_impl<T, indexes...>& v)
{
    for (size_t i = 0; i < sizeof...(indexes); ++i)
    {
        out << v[i] << " ";
    }
    out << "\n";
    return out;
}

using vec3f = vec_impl<float, 0, 1, 2>;
using vec2f = vec_impl<float, 0, 1>;
using vec3i = vec_impl<int, 0, 1, 2>;
using vec4f = vec_impl<float, 0, 1, 2, 3>;
}
