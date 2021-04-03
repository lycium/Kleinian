#pragma once

#include "real.h"
#include "Dual.h"
#include "Complex.h"



template <typename complex_type>
struct Moebius
{
	complex_type a, b, c, d;



	Moebius() noexcept { }
	Moebius(const complex_type & a_, const complex_type & b_, const complex_type & c_, const complex_type & d_) noexcept : a(a_), b(b_), c(c_), d(d_) { }

	Moebius operator+(const Moebius & rhs) const noexcept { Moebius m; m.a = a + rhs.a; m.b = b + rhs.b; m.c = c + rhs.c; m.d = d + rhs.d; return m; }
	Moebius operator-(const Moebius & rhs) const noexcept { Moebius m; m.a = a - rhs.a; m.b = b - rhs.b; m.c = c - rhs.c; m.d = d - rhs.d; return m; }

	// Matrix multiplication
	Moebius operator*(const Moebius & rhs) const noexcept
	{
		return Moebius(
			a * rhs.a + b * rhs.c,
			a * rhs.b + b * rhs.d,
			c * rhs.a + d * rhs.c,
			c * rhs.b + d * rhs.d);
	}


	Moebius inverse() const noexcept { return Moebius(a, -b, -c, d); }

	static Moebius ident() noexcept { return Moebius(complex_type(1), complex_type(0), complex_type(0), complex_type(1)); }

	complex_type apply(const complex_type & z) const noexcept
	{
		const complex_type num = a * z + b;
		const complex_type den = c * z + d;
		return num / den;
	}

	complex_type applyInverse(const complex_type & z) const noexcept
	{
		const complex_type num = a * z - b;
		const complex_type den = d - c * z;
		return num / den;
	}
};


using Moebiusr = Moebius<Complexr>;
using Moebiusf = Moebius<Complexf>;
using Moebiusd = Moebius<Complexd>;
