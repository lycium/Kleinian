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
	using Moebius_type = Moebius<complex_type>;

	std::vector<Moebius_type> xfs;


	void ident()
	{
		xfs.resize(0);
		xfs.push_back(Moebius_type(1, 0, 0, 1));
	}

	void addMoebius(const Moebius_type & m)
	{
		xfs.push_back(m);
		xfs.push_back(m.inverse());
	}

	void init(const Moebius_type & a, const Moebius_type & b)
	{
		xfs.resize(0);

		addMoebius(a);
		addMoebius(b);
	}

	void initSimple(const Complexr & mu)
	{
		const Moebius_type a(0, 1, -1, mu);
		const Moebius_type b(0, 1, 1, Complexr(0, 1));

		init(a, b);

		// This xform is sort of the complement of b and fills the hyperbolic plane
		const Moebius_type b_compl(0, 1, 1, -Complexr(0, 1));
		addMoebius(b_compl);
	}
};

int main(int argc, char ** argv)
{
	constexpr int multi = 1;
	constexpr int xres  = multi * 1024;
	constexpr int yres  = multi * 1024;
	constexpr size_t iter_depth = 1 << 10;
	constexpr float aspect = yres / float(xres);

	const int start_shape = 2;
	const int n0 = 5;
	const Complexr mu(2 * std::cos(3.141592653589793238 / n0));
	//const Complexr mu(1.748913278003935259, 0.224911048597954425);
	//const Complexr mu(1.886141097183404, 0.06744837715485395);

	Kleinian<Complexr> klein;
	klein.initSimple(mu);

	const uint32_t num_xfs = klein.xfs.size(); 

	// The iteration is actually happing in half-plane model, but we want to display the image in disc model.
	// So we have a "final transform" applied before plotting (not iterated).
	const Moebius<Complexr> camera_xf(Complexr(1), Complexr(-1), Complexr(1), Complexr(1));
	//const Moebius<Complexr> camera_xf(Complexr(1), Complexr(0), Complexr(0), Complexr(1));

	std::vector<rgba>  image(xres * yres);
	std::vector<vec4f> histo(xres * yres, vec4f(0.0f));

	pcg32 rng(1337666);

	size_t completed_iters = 0;
	size_t num_iters = 1 << 16;

	while (true)
	{
		for (size_t iter = 0; iter < num_iters; ++iter)
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
				case 2: // Random point on y axis
				{
					const real a0 = rng.nextDouble() * 6.283185307179586476925286766559;
					//z = Complexr(std::cos(a0), std::sin(a0)); // Start on cirlcle
					//z = (z + Complexr(1, 0)) / (Complexr(1, 0) - z); // Transform to y axis
					z = Complexr(0, std::tan(a0)); // Simplifies to this
					break;
				}
				default:
					break;
			};

			int current_xf = rng.nextUInt(num_xfs); // Select first transform to apply uniformly

			for (int depth = 0; depth < iter_depth; ++depth)
			{
				//if (depth < 64) continue;

				const Complexr z_plot = camera_xf.apply(z);
				const real cam_scale = 1;

				// Plot point
				const real x_pixel = (xres * 0.5) * cam_scale + ( z_plot.ri[0]) * (xres * 0.5) * cam_scale * aspect;
				const real y_pixel = (yres * 0.5) * cam_scale + (-z_plot.ri[1]) * (yres * 0.5) * cam_scale;
				const int x_i = int(x_pixel + rng.nextDouble() - 0.5);
				const int y_i = int(y_pixel + rng.nextDouble() - 0.5);
				if (x_i >= 0 && x_i < xres && y_i >= 0 && y_i < yres)
				{
					const int pixel_idx = y_i * xres + x_i;
					histo[pixel_idx] += vec4f(1.0f);
				}

				const Complexr next_z = klein.xfs[current_xf].apply(z);
				const int next_xf = rng.nextUInt(num_xfs); // Select next transform to apply uniformly

				z = next_z;
				current_xf = next_xf;
			}
		}

		completed_iters += num_iters;
		const float img_scale = xres * yres / float(completed_iters * iter_depth);

		for (int y = 0; y < yres; y++)
		for (int x = 0; x < xres; x++)
		{
			const int pixel_idx = y * xres + x;
			const float v = histo[pixel_idx].e[0] * img_scale;
			const float c = std::log(v * 256.0f + 1) * 24.0f;

			const int int_c = std::max(0, std::min(255, int(c)));
			image[pixel_idx].r = int_c;
			image[pixel_idx].g = int_c;
			image[pixel_idx].b = int_c;
			image[pixel_idx].a = 255;
		}

		stbi_write_png("out.png", xres, yres, 4, &image[0], xres * 4);
		printf("saved image with %d iters\n", completed_iters);

		num_iters *= 2;
	}


	return 0;
}
