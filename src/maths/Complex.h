#pragma once

#include "maths/real.h"
#include "maths/Dual.h"
#include <math.h>



template <typename real_type>
struct Complex
{
	real_type ri[2];


	constexpr Complex() noexcept { }
	constexpr Complex(const Complex & v) noexcept { for (int i = 0; i < 2; ++i) ri[i] = v.ri[i]; }
	constexpr Complex(const real_type & v) noexcept { for (int i = 0; i < 2; ++i) ri[i] = v; }

	// Some brutal C++ hackery to enable initializer lists
	template<typename val, typename... vals, std::enable_if_t<(sizeof...(vals) > 0), int> = 0>
	constexpr Complex(const val v, const vals... vs) noexcept : ri { (real_type)v, (real_type)vs... } { }

	constexpr Complex operator+(const Complex & rhs) const noexcept { Complex r; for (int i = 0; i < 2; ++i) r.ri[i] = ri[i] + rhs.ri[i]; return r; }
	constexpr Complex operator-(const Complex & rhs) const noexcept { Complex r; for (int i = 0; i < 2; ++i) r.ri[i] = ri[i] - rhs.ri[i]; return r; }
	constexpr Complex operator*(const real_type & rhs) const noexcept { Complex r; for (int i = 0; i < 2; ++i) r.ri[i] = ri[i] * rhs; return r; }
	constexpr Complex operator/(const real_type & rhs) const noexcept { return *this * (real_type(1) / rhs); }

	constexpr Complex operator*(const Complex & rhs) const noexcept
	{
		return Complex(
			ri[0] * rhs.ri[0] - ri[1] * rhs.ri[1],
			ri[0] * rhs.ri[1] + ri[1] * rhs.ri[0]);
	}

	constexpr Complex operator/(const Complex & rhs) const noexcept
	{
		const real_type r = ri[0] * rhs.ri[0] + ri[1] * rhs.ri[1];
		const real_type i = ri[1] * rhs.ri[0] - ri[0] * rhs.ri[1];
		return Complex(r, i) * (1 / rhs.length2());
	}

	constexpr Complex operator-() const noexcept { Complex r; for (int i = 0; i < 2; ++i) r.ri[i] = -ri[i]; return r; }

	constexpr const Complex & operator =(const Complex & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i]  = rhs.ri[i]; return *this; }
	constexpr const Complex & operator+=(const Complex & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i] += rhs.ri[i]; return *this; }
	constexpr const Complex & operator-=(const Complex & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i] -= rhs.ri[i]; return *this; }
	constexpr const Complex & operator*=(const Complex & rhs) noexcept { *this = *this * rhs; return *this; }
	constexpr const Complex & operator/=(const Complex & rhs) noexcept { *this = *this / rhs; return *this; }

	constexpr const Complex & operator =(const real_type & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i]  = rhs; return *this; }
	constexpr const Complex & operator+=(const real_type & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i] += rhs; return *this; }
	constexpr const Complex & operator-=(const real_type & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i] -= rhs; return *this; }
	constexpr const Complex & operator*=(const real_type & rhs) noexcept { for (int i = 0; i < 2; ++i) ri[i] *= rhs; return *this; }
	constexpr const Complex & operator/=(const real_type & rhs) noexcept { const real_type s = real_type(1) / rhs; for (int i = 0; i < 2; ++i) ri[i] *= s; return *this; }

	constexpr real_type length2() const noexcept { return ri[0] * ri[0] + ri[1] * ri[1]; }
	constexpr real_type length()  const noexcept { return std::sqrt(length2()); }
};


using Complexr = Complex<real>;
using Complexf = Complex<float>;
using Complexd = Complex<double>;
