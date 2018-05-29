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

#ifndef SAURON_SKY_RENDERER_CORE_UTIL_HPP
#define SAURON_SKY_RENDERER_CORE_UTIL_HPP

#pragma once

#include <cmath>
#include <cstdint>

#include <boost/assert.hpp>

#if defined(DEBUG) | defined(_DEBUG)
	#define SAURON_DEBUG
#endif

#define SAURON_UNUSED(x) (void)(x)

namespace Sauron
{
	using std::uint64_t;
	using std::uint32_t;
	using std::uint16_t;
	using std::uint8_t;
	using std::int64_t;
	using std::int32_t;
	using std::int16_t;
	using std::int8_t;

	static double constexpr AU = 149597870.691;
	static double constexpr SPEED_OF_LIGHT = 299792.458;

	template <typename To, typename From>
	inline To checked_cast(From p) noexcept
	{
		BOOST_ASSERT(dynamic_cast<To>(p) == static_cast<To>(p));
		return static_cast<To>(p);
	}

	template <typename T>
	inline T Deg2Rad(T degrees)
	{
		return static_cast<T>(degrees * (M_PI / 180));
	}

	template <typename T>
	inline T Rad2Deg(T radians)
	{
		return static_cast<T>(radians * (180 / M_PI));
	}

	//! Compute cosines and sines around a circle which is split in "segments" parts.
	//! Values are stored in the global static array cos_sin_theta.
	//! Used for the sin/cos values along a latitude circle, equator, etc. for a spherical mesh.
	//! @param slices number of partitions (elsewhere called "segments") for the circle
	float* ComputeCosSinTheta(int slices);

	//! Compute cosines and sines around a half-circle which is split in "segments" parts.
	//! Values are stored in the global static array cos_sin_rho.
	//! Used for the sin/cos values along a meridian for a spherical mesh.
	//! @param segments number of partitions (elsewhere called "stacks") for the half-circle
	float* ComputeCosSinRho(int segments);

	//! Compute cosines and sines around part of a circle (from top to bottom) which is split in "segments" parts.
	//! Values are stored in the global static array cos_sin_rho.
	//! Used for the sin/cos values along a meridian.
	//! This allows leaving away pole caps. The array now contains values for the region minAngle+segments*phi
	//! @param delta_rho a difference angle between the stops
	//! @param segments number of segments
	//! @param min_angle start angle inside the half-circle. maxAngle=minAngle+segments*phi
	float* ComputeCosSinRhoZone(float delta_rho, int segments, float min_angle);
}

#endif		// SAURON_SKY_RENDERER_CORE_UTIL_HPP
