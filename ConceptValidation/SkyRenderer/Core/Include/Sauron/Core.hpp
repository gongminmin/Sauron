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

#ifndef SAURON_SKY_RENDERER_CORE_CORE_HPP
#define SAURON_SKY_RENDERER_CORE_CORE_HPP

#pragma once

#include <memory>

#include <Sauron/Projector.hpp>
#include <Sauron/SkyDrawer.hpp>

namespace Sauron
{
	class Location;
	class Observer;

	//! @class Core
	//! Main class for Sauron sky renderer core processing.
	class Core
	{
	public:
		//! @enum RefractionMode
		//! Available refraction mode.
		enum class RefractionMode
		{
			Auto,                     //!< Automatically decide to add refraction if atmosphere is activated
			On,                       //!< Always add refraction (i.e. apparent coordinates)
			Off                       //!< Never add refraction (i.e. geometric coordinates)
		};

		static double constexpr JD_SECOND = 1.0 / (24 * 60 * 60);

	public:
		Core();

		void Init();

		//! Update all the objects with respect to the time.
		//! @param delta_time the time increment in sec.
		void Update(double delta_time);

		//! Handle the resizing of the window
		void WindowHasBeenResized(float x, float y, float width, float height);

		//! Update core state before drawing modules.
		void PreDraw();

		//! Update core state after drawing modules.
		void PostDraw();

		//! Draw the sky
		void Draw();

		//! Replaces the current observer.
		void SetCurrentObserver(std::shared_ptr<Observer> const & ob);

		//! Returns the current observer.
		std::shared_ptr<Observer> const & GetCurrentObserver() const;

		//! Move the observer to the given location
		//! @param target the target location
		void MoveObserverTo(Location const & target);

		//! Get a new instance of projector using the given modelview transformation.
		//! If not specified the projection used is the one currently used as default.
		std::shared_ptr<Projector> GetProjection(std::shared_ptr<Projector::ModelViewTransform> transform) const;

		//! Get the current set of parameters to use when creating a new StelProjector.
		Projector::ProjectorParams GetCurrentProjectorParams() const;

		//! Set the set of parameters to use when creating a new StelProjector.
		void SetCurrentProjectorParams(Projector::ProjectorParams const & params);

		//! Get the current SkyDrawer used in the core.
		SkyDrawer* GetSkyDrawer();

		//! Get the current StelSkyDrawer used in the core.
		SkyDrawer const * GetSkyDrawer() const;

		//! Set vision direction
		void LookAtJ2000(glm::dvec3 const & pos, glm::dvec3 const & up);

		glm::dvec3 AltAzToJ2000(glm::dvec3 const & v, RefractionMode ref_mode = RefractionMode::Auto) const;
		glm::dvec3 J2000ToAltAz(glm::dvec3 const & v, RefractionMode ref_mode = RefractionMode::Auto) const;

		//! Get the modelview matrix for observer-centric J2000 equatorial drawing.
		std::shared_ptr<Projector::ModelViewTransform> GetJ2000ModelViewTransform(RefractionMode ref_mode = RefractionMode::Auto) const;

		//! Rotation matrix from equatorial J2000 to ecliptic (VSOP87A).
		static glm::dmat4 const & GetMatJ2000ToVsop87();
		//! Rotation matrix from ecliptic (VSOP87A) to equatorial J2000.
		static glm::dmat4 const & GetMatVsop87ToJ2000();

		//! Set the current date in Julian Day (UT)
		void SetJD(double new_jd);
		//! Set the current date in Julian Day (TT).
		//! The name is derived from the classical name "Ephemeris Time", of which TT is the successor.
		//! It is still frequently used in the literature.
		void SetJDE(double new_jde);
		//! Get the current date in Julian Day (UT).
		double GetJD() const;
		//! Get the current date in Julian Day (TT).
		//! The name is derived from the classical name "Ephemeris Time", of which TT is the successor.
		//! It is still frequently used in the literature.
		double GetJDE() const;

		double ComputeDeltaT(double jd);

		//! @return whether nutation is currently used.
		bool GetUseNutation() const
		{
			return use_nutation_;
		}
		//! Set whether you want computation and simulation of nutation (a slight wobble of Earth's axis, just a few arcseconds).
		void SetUseNutation(bool use)
		{
			use_nutation_ = use;
		}

	private:
		void UpdateTransformMatrices();
		void UpdateTime(double delta_time);
		void ResetSync();

	private:
		std::unique_ptr<SkyDrawer> sky_drawer_;

		std::shared_ptr<Observer> curr_observer_;
		Projector::ProjectorParams curr_projector_params_;

		bool use_nutation_ = true;

		// Matrices used for every coordinate transfo
		glm::dmat4 mat_heliocentric_ecliptic_j2000_to_alt_az_;	// Transform from heliocentric ecliptic Cartesian (VSOP87A) to topocentric (StelObserver) altazimuthal coordinate
		glm::dmat4 mat_alt_az_to_heliocentric_ecliptic_j2000_;	// Transform from topocentric (StelObserver) altazimuthal coordinate to heliocentric ecliptic Cartesian (VSOP87A)
		glm::dmat4 mat_alt_az_to_equinox_equ_;					// Transform from topocentric altazimuthal coordinate to Earth Equatorial
		glm::dmat4 mat_equinox_equ_to_alt_az_;					// Transform from Earth Equatorial to topocentric (Observer) altazimuthal coordinate
		glm::dmat4 mat_heliocentric_ecliptic_to_equinox_equ_;	// Transform from heliocentric ecliptic Cartesian (VSOP87A) to earth equatorial coordinate
		glm::dmat4 mat_equinox_equ_to_j2000_;					// For Earth, this is almost the inverse precession matrix, =Rz(VSOPbias)Rx(eps0)Rz(-psiA)Rx(-omA)Rz(chiA)
		glm::dmat4 mat_j2000_to_equinox_equ_;					// precession matrix

		glm::dmat4 mat_j2000_to_alt_az_;
		glm::dmat4 mat_alt_az_to_j2000_;

		glm::dmat4 mat_alt_az_model_view_;			// Modelview matrix for observer-centric altazimuthal drawing
		glm::dmat4 invert_mat_alt_az_model_view_;	// Inverted modelview matrix for observer-centric altazimuthal drawing

		// flag to indicate that we show topocentrically corrected coordinates. (Switching to false for planetocentric coordinates is new for 0.14)
		bool use_topocentric_coordinates_;

		std::pair<double, double> julian_day_;		// first=JD_UT, second=DeltaT=TT-UT. To gain JD_TT, compute JDE=JD.first+JD.second or better just call GetJDE()
		uint64_t milli_seconds_of_last_jd_update_;	// Time in seconds when the time rate or time last changed
		double jd_of_last_jd_update_;				// JD when the time rate or time last changed
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_LOCATION_HPP
