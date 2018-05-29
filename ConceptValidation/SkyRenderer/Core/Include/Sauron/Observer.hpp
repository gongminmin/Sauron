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

#ifndef SAURON_SKY_RENDERER_CORE_OBSERVER_HPP
#define SAURON_SKY_RENDERER_CORE_OBSERVER_HPP

#pragma once

#include <Sauron/Location.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Sauron
{
	class Planet;

	//! @class Observer
	class Observer
	{
	public:
		//! Create a new Observer instance which is at a fixed Location
		explicit Observer(Location const & loc);

		//! Get the position of the home planet center in the heliocentric VSOP87 frame in AU
		glm::dvec3 GetCenterVsop87Pos() const;
		//! Get the distance between observer and home planet center in AU.
		//! This is distance &rho; from Meeus, Astron. Algorithms, 2nd edition 1998, ch.11, p.81f.
		double GetDistanceFromCenter() const;
		//! Get the geocentric rectangular coordinates of the observer in AU, plus geocentric latitude &phi;'.
		//! This is vector &rho; from Meeus, Astron. Algorithms, 2nd edition 1998, ch.11, p.81f.
		//! The first component is &rho; cos &phi;' [AU], the second component is &rho; sin &phi&' [AU], the third is &phi;' [radians].
		glm::dvec3 GetTopographicOffsetFromCenter() const;

		//! returns rotation matrix for conversion of alt-azimuthal to equatorial coordinates
		//! For Earth we need JD(UT), for other planets JDE! To be general, just have both in here!
		glm::dmat4 GetRotAltAzToEquatorial(double jd, double jde) const;
		glm::dmat4 GetRotEquatorialToVsop87() const;

		std::shared_ptr<Planet> const & GetHomePlanet() const;

		//! Get the informations on the current location
		Location const & GetCurrentLocation() const
		{
			return curr_location_;
		}

	protected:
		Location curr_location_;
		std::shared_ptr<Planet> planet_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_OBSERVER_HPP
