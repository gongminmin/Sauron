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

#ifndef SAURON_SKY_RENDERER_CORE_MODULES_PLANET_HPP
#define SAURON_SKY_RENDERER_CORE_MODULES_PLANET_HPP

#pragma once

#include <Sauron/Core.hpp>

#include <string>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Sauron
{
	// The callback type for the external position computation function
	// arguments are JDE, position[3], velocity[3].
	// The last variable is the userData pointer.
	typedef void (*PosFunc)(double jd, double xyz[3], double xyzdot[3], void* param);

	// epoch J2000: 12 UT on 1 Jan 2000
	double constexpr J2000 = 2451545.0;
	int constexpr ORBIT_SEGMENTS = 360;

	// Class used to store rotational elements, i.e. axis orientation for the planetary body.
	struct RotationElements
	{
		float period = 1;			// (sidereal) rotation period [earth days]
		float offset = 0;			// rotation at epoch  [degrees]
		double epoch = J2000;		// JDE (JD TT) of epoch for these elements
		float obliquity = 0;		// tilt of rotation axis w.r.t. ecliptic [radians]
		float ascending_node = 0;	// long. of ascending node of equator on the ecliptic [radians]
		float precession_rate = 0;	// rate of precession of rotation axis in [rads/JulianCentury(36525d)]
		double sidereal_period = 0;	// sidereal period (Planet year or a moon's sidereal month) [earth days]
	};

	// TODO: Rename to AstronomicalObject?
	class Planet
	{
		friend class SolarSystem;

	public:
		Planet(std::string const & english_name,
			double radius,
			double oblateness,
			PosFunc coord_func,
			void* orbit_ptr);

		//! Get the equator radius of the planet in AU.
		//! @return the equator radius of the planet in astronomical units.
		double GetRadius() const
		{
			return radius_;
		}

		//! Get the value (1-f) for oblateness f.
		double GetOneMinusOblateness() const
		{
			return one_minus_oblateness_;
		}

		//! Compute the z rotation to use from equatorial to geographic coordinates. For general applicability we need both time flavors:
		//! @param jd is JD(UT) for Earth
		//! @param jde is used for other locations
		double GetSiderealTime(double jd, double jde) const;
		glm::dmat4 GetRotEquatorialToVsop87() const;
		void SetRotEquatorialToVsop87(glm::dmat4 const & m);

		RotationElements const & GetRotationElements() const 
		{
			return re_;
		}
		// Set the orbital elements
		void SetRotationElements(float period, float offset, double epoch,
			float obliquity, float ascending_node,
			float precession_rate, double sidereal_period);

		//! Compute the position in the parent Planet coordinate system
		void ComputePositionWithoutOrbits(double date_jde);
		void ComputePosition(double date_jde);

		//! Compute the transformation matrix from the local Planet coordinate to the parent Planet coordinate.
		//! This requires both flavours of JD in cases involving Earth.
		void ComputeTransMatrix(double jd, double jde);

		//! Get the Planet position in the parent Planet ecliptic coordinate in AU
		glm::dvec3 const & GetEclipticPos() const
		{
			return ecliptic_pos_;
		}
		
		//! Return the heliocentric ecliptical position
		glm::dvec3 GetHeliocentricEclipticPos() const
		{
			return this->GetHeliocentricPos(ecliptic_pos_);
		}

		//! Return the heliocentric transformation for local coordinate
		glm::dvec3 GetHeliocentricPos(glm::dvec3 const & pos) const;
		void SetHeliocentricEclipticPos(glm::dvec3 const & pos);

		//! Get the planet velocity around the parent planet in ecliptical coordinates in AU/d
		glm::dvec3 GetEclipticVelocity() const
		{
			return ecliptic_velocity_;
		}

		std::shared_ptr<Planet> const & GetParent() const
		{
			return parent_;
		}

	private:
		std::string english_name_;			// Planet name in English
		RotationElements re_;				// Rotation param
		double radius_;						// Planet radius in AU
		double one_minus_oblateness_;		// (polar radius) / (equatorial radius)
		glm::dvec3 ecliptic_pos_ = glm::dvec3(0, 0, 0);	// Position in AU in the rectangular ecliptic coordinate system (J2000) around the parent body.
														// To get heliocentric coordinates, use GetHeliocentricEclipticPos()
		glm::dvec3 ecliptic_velocity_ = glm::dvec3(0, 0, 0);	// Speed in AU/d in the rectangular ecliptic coordinate system (J2000) around the parent body.
																// To get velocity, preferrably read getEclipticVelocity() and getHeliocentricEclipticVelocity()
																// The "State Vector" [Heafner 1999] can be formed from (JDE, eclipticPos, eclipticVelocity)

		glm::dmat4 rot_local_to_parent_;
		// Apparently this is the axis orientation with respect to the parent body. For planets, this is axis orientation w.r.t. VSOP87A/J2000 ecliptical system.
		float axis_rotation_;				// Rotation angle of the Planet on its axis.
		double last_jde_ = J2000;			// caches JDE of last positional computation
											// The callback for the calculation of the equatorial rect heliocentric position at time JDE.
		PosFunc coord_func_;
		void* orbit_ptr_;					// this is always used with an Orbit object.
		std::shared_ptr<Planet> parent_;	// Planet parent i.e. sun for earth

		double last_orbit_jde_ = 0;
		double delta_jde_ = Core::JD_SECOND;	// time difference between positional updates.
		double delta_orbit_jde_ = 0;
		bool orbit_cached_ = false;				// whether orbit calculations are cached for drawing orbit yet
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_MODULES_PLANET_HPP
