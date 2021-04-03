#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

#include "maths/real.h"
#include "maths/Dual.h"
#include "maths/Complex.h"
#include "maths/vec.h"
#include "maths/Moebius.h"

#include "util/pcg32.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "util/stb_image_write.h"

#include <vector>



struct rgba
{
	uint8_t r, g, b, a;
};


template <typename complex_type>
struct Kleinian
{
	Moebius<complex_type> abcd[4];

	void initBlah()
	{
		abcd[0] = Moebius<complex_type>(1, 0, 0, 1);
		abcd[1] = Moebius<complex_type>(1, 0, 0, 1);
		abcd[2] = Moebius<complex_type>(1, 0, 0, 1);
		abcd[3] = Moebius<complex_type>(1, 0, 0, 1);
	}
};


int main(int argc, char ** argv)
{
	constexpr int xres = 1024;
	constexpr int yres = 640;
	constexpr int num_iters = 1 << 20;
	constexpr int iter_depth = 1 << 8;
	constexpr float aspect = yres / float(xres);

	const int start_shape = 1;
	Kleinian<Complexr> klein;
	klein.initBlah();

	// The iteration is actually happing in half-plane model, but we want to display the image in disc model.
	// So we have a "final transform" applied before plotting (not iterated).
	const Moebius<Complexr> camera_xf { 1, -1, 1, 1 };

	std::vector<rgba>  image(xres * yres);
	std::vector<vec4f> histo(xres * yres, vec4f(0.0f));

	pcg32 rng(1337666);

	//if (false)
	for (int iter = 0; iter < num_iters; ++iter)
	{
		Complexr z;
		switch (start_shape)
		{
			case 0: // Random point on circle
			{
				const real a0 = rng.nextDouble() * 6.283185307179586476925286766559;
				z = Complexr(std::cos(a0), std::sin(a0));
				break;
			}
			case 1: // Random point on disc
			{
				const real a0 = rng.nextDouble() * 6.283185307179586476925286766559;
				const real r0 = std::sqrt(rng.nextDouble());
				z = Complexr(std::cos(a0), std::sin(a0)) * r0;
				break;
			}
			case 2: // Random on y axis
			{
				const real a0 = rng.nextDouble() * 6.283185307179586476925286766559;
				//z = Complexr(std::cos(a0), std::sin(a0)); // Start on cirlcle
				//z = (z + Complexr(1, 0)) / (Complexr(1, 0) - z); // Transform to y axis
				z = Complexr(std::tan(a0), 0); // Simplifies to this
				break;
			}
			default:
				break;
		};

		int current_xf = rng.nextUInt(4); // Select first transform to apply uniformly

		for (int depth = 0; depth < iter_depth; ++depth)
		{
			const Complexr z_plot = camera_xf.apply(z);
			// Plot point
			const real x_pixel = (xres * 0.5) +  z_plot.ri[0] * (xres * 0.5) * aspect;
			const real y_pixel = (yres * 0.5) + -z_plot.ri[1] * (yres * 0.5);
			const int x_i = int(x_pixel);
			const int y_i = int(y_pixel);
			if (x_i >= 0 && x_i < xres && y_i >= 0 && y_i < yres)
			{
				const int pixel_idx = y_i * xres + x_i;
				histo[pixel_idx] += vec4f(1.0f);
			}

			const Complexr next_z = klein.abcd[current_xf].apply(z);
			const int next_xf = rng.nextUInt(4); // Select next transform to apply uniformly

			z = next_z;
			current_xf = next_xf;
		}
	}

	for (int y = 0; y < yres; y++)
	for (int x = 0; x < xres; x++)
	{
		const int pixel_idx = y * xres + x;
		const float v = histo[pixel_idx].e[0] + 1;
		const float c = std::log(v) * 12.0f;

		const int int_c = std::max(0, std::min(255, int(c)));
		image[pixel_idx].r = int_c;
		image[pixel_idx].g = int_c;
		image[pixel_idx].b = int_c;
		image[pixel_idx].a = 255;
	}

	stbi_write_png("out.png", xres, yres, 4, &image[0], xres * 4);

	return 0;
}
