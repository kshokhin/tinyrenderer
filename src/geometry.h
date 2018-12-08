#include<iostream>
#include<array>
#include<cstdint>
#include<cmath>

namespace sk
{
template<typename T, size_t ...indexes>
class vec_impl
{
public:
    constexpr vec_impl() { (m_data[indexes] = ... = 0); }
    constexpr vec_impl(const vec_impl& rhs) { ((m_data[indexes] = rhs.m_data[indexes]), ...);}
    template <typename...Ts>
    constexpr vec_impl(Ts ...vals) { static_assert(sizeof...(vals) == sizeof...(indexes)); ((m_data[indexes] = vals), ...);}

    constexpr T& operator[](size_t i) { return m_data[i]; }
    constexpr const T& operator[](size_t i) const { return m_data[i];}
    constexpr T len() const { return std::sqrt(dot(*this, *this)); }
    constexpr void norm() { auto l = len(); ((m_data[indexes] /= l), ...); };
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

    res[0] = lhs[1]*rhs[2] - lhs[2]*rhs[1];
    res[1] = lhs[2]*rhs[0] - lhs[0]*rhs[2];
    res[2] = lhs[0]*rhs[1] - lhs[1]*rhs[0];

    return res;
}

using vec3f = vec_impl<float, 0, 1, 2>;
using vec3i = vec_impl<int, 0, 1, 2>;
}
