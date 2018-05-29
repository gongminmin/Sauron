/*
 * Copyright (C) 2018 Minmin Gong
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Sauron/Util.hpp>

#include <boost/assert.hpp>

namespace Sauron
{
	// Arrays to keep cos/sin of angles and multiples of angles. rho and theta are delta angles, and these arrays
	static int constexpr MAX_STACKS = 4096;
	static float cos_sin_rho[2 * (MAX_STACKS + 1)];
	static int constexpr MAX_SLICES = 4096;
	static float cos_sin_theta[2 * (MAX_SLICES + 1)];

	//! Compute cosines and sines around a circle which is split in "segments" parts.
	//! Values are stored in the global static array cos_sin_theta.
	//! Used for the sin/cos values along a latitude circle, equator, etc. for a spherical mesh.
	//! @param slices number of partitions (elsewhere called "segments") for the circle
	float* ComputeCosSinTheta(int slices)
	{
		BOOST_ASSERT(slices <= MAX_SLICES);

		// Difference angle between the stops. Always use 2 * PI / slices!
		float const delta_theta = static_cast<float>(2 * M_PI / slices);
		float *cos_sin = cos_sin_theta;
		float *cos_sin_rev = cos_sin + 2 * (slices + 1);
		const float c = std::cos(delta_theta);
		const float s = std::sin(delta_theta);
		*cos_sin++ = 1;
		*cos_sin++ = 0;
		*-- cos_sin_rev = -cos_sin[-1];
		*-- cos_sin_rev = cos_sin[-2];
		*cos_sin ++ = c;
		*cos_sin ++ = s;
		*-- cos_sin_rev = -cos_sin[-1];
		*-- cos_sin_rev = cos_sin[-2];
		while (cos_sin < cos_sin_rev)   // compares array address indices only!
		{
			// avoid expensive trig functions by use of the addition theorem.
			cos_sin[0] = cos_sin[-2] * c - cos_sin[-1] * s;
			cos_sin[1] = cos_sin[-2] * s + cos_sin[-1] * c;
			cos_sin += 2;
			*-- cos_sin_rev = -cos_sin[-1];
			*-- cos_sin_rev = cos_sin[-2];
		}
		return cos_sin_theta;
	}

	//! Compute cosines and sines around a half-circle which is split in "segments" parts.
	//! Values are stored in the global static array cos_sin_rho.
	//! Used for the sin/cos values along a meridian for a spherical mesh.
	//! @param segments number of partitions (elsewhere called "stacks") for the half-circle
	float* ComputeCosSinRho(int segments)
	{
		BOOST_ASSERT(segments <= MAX_STACKS);

		// Difference angle between the stops. Always use PI / segments!
		float const delta_rho = static_cast<float>(M_PI / segments);
		float *cos_sin = cos_sin_rho;
		float *cos_sin_rev = cos_sin + 2 * (segments + 1);
		float const c = std::cos(delta_rho);
		float const s = std::sin(delta_rho);
		*cos_sin ++ = 1.f;
		*cos_sin ++ = 0.f;
		*-- cos_sin_rev = cos_sin[-1];
		*-- cos_sin_rev = -cos_sin[-2];
		*cos_sin ++ = c;
		*cos_sin ++ = s;
		*-- cos_sin_rev = cos_sin[-1];
		*-- cos_sin_rev = -cos_sin[-2];
		while (cos_sin < cos_sin_rev)    // compares array address indices only!
		{
			// avoid expensive trig functions by use of the addition theorem.
			cos_sin[0] = cos_sin[-2] * c - cos_sin[-1] * s;
			cos_sin[1] = cos_sin[-2] * s + cos_sin[-1] * c;
			cos_sin += 2;
			*-- cos_sin_rev = cos_sin[-1];
			*-- cos_sin_rev = -cos_sin[-2];
		}

		return cos_sin_rho;
	}

	//! Compute cosines and sines around part of a circle (from top to bottom) which is split in "segments" parts.
	//! Values are stored in the global static array cos_sin_rho.
	//! Used for the sin/cos values along a meridian.
	//! This allows leaving away pole caps. The array now contains values for the region minAngle+segments*phi
	//! @param dRho a difference angle between the stops
	//! @param segments number of segments
	//! @param minAngle start angle inside the half-circle. maxAngle=minAngle+segments*phi
	float* ComputeCosSinRhoZone(float delta_rho, int segments, float min_angle)
	{
		float *cos_sin = cos_sin_rho;
		float const c = cos(delta_rho);
		float const s = sin(delta_rho);
		*cos_sin ++ = cos(min_angle);
		*cos_sin ++ = sin(min_angle);
		for (int i = 0; i < segments; ++ i) // we cannot mirror this, it may be unequal.
		{
			// efficient computation, avoid expensive trig functions by use of the addition theorem.
			cos_sin[0] = cos_sin[-2] * c - cos_sin[-1] * s;
			cos_sin[1] = cos_sin[-2] * s + cos_sin[-1] * c;
			cos_sin += 2;
		}
		return cos_sin_rho;
	}
}
