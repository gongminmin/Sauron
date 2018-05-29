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

#ifndef SAURON_SKY_RENDERER_CORE_REFRACTION_EXTINCTION_HPP
#define SAURON_SKY_RENDERER_CORE_REFRACTION_EXTINCTION_HPP

#pragma once

// USABILITY: added 3 more flags/switches:
// Temperature [C] [influences refraction]
// Pressure [mbar]  [influences refraction]
// extinction Coeff. k=0...(0.01)...1, [if k=0, no ext. effect]
// SUGGESTION: Allow Temperature/Pressure/ex.Coeff./LightPollution set in the landscape files

#include <Sauron/Projector.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Sauron
{
	//! @class Extinction
	//! This class performs extinction computations, following literature from atmospheric optics and astronomy.
	//! Airmass computations are limited to meaningful altitudes.
	//! The solution provided here will [hopefully!] result in a visible "zone of avoidance" near the horizon down to altitude -2,
	//! and may show stars in their full brightness below -2 degrees.
	//! Typical horizons do not go down below -1, so all natural sites should be covered.
	//! Note that forward/backward are no absolute reverse operations!
	//! All the computations should be in effect
	//! (1) only if atmosphere effects are true
	//! (2) only for terrestrial locations, not on Moon/Mars/Saturn etc
	//! config.ini:astro/flag_extinction_below_horizon=true|false controls if extinction kills objects below -2 degrees altitude by setting airmass to 42.
	class Extinction
	{
	public:
		//! Define the extinction strategy for rendering underground objects (useful when ground is not rendered)
		enum class UndergroundExtinctionMode
		{
			Zero = 0,	//!< Zero extinction: stars visible in full brightness
			Max = 1,   //!< Maximum extinction: coef 42, i.e practically invisible
			Mirror = 2 //!< Mirror the extinction for the same altutide above the ground.
		};

		Extinction() = default;

		//! Compute extinction effect for arrays of size num position vectors and magnitudes.
		//! @param altAzPos are the NORMALIZED (!!) (geometrical) star position vectors, and their z components sin(geometric_altitude).
		//! This call must therefore be done before application of Refraction if atmospheric effects are on.
		//! Note that forward/backward are no absolute reverse operations!
		void Forward(glm::dvec3 const & alt_az_pos, float* mag) const;

		//! Compute inverse extinction effect for arrays of size num position vectors and magnitudes.
		//! @param altAzPos are the NORMALIZED (!!) (geometrical) star position vectors, and their z components sin(geometric_altitude).
		//! Note that forward/backward are no absolute reverse operations!
		void Backward(glm::dvec3 const & alt_az_pos, float* mag) const;

		//! Get visual extinction coefficient (mag/airmass), influences extinction computation.
		float GetExtinctionCoefficient() const
		{
			return ext_coeff_;
		}

		//! Set visual extinction coefficient (mag/airmass), influences extinction computation.
		//! @param k= 0.1 for highest mountains, 0.2 for very good lowland locations, 0.35 for typical lowland, 0.5 in humid climates.
		void SetExtinctionCoefficient(float k)
		{
			ext_coeff_ = k;
		}

		void SetUndergroundExtinctionMode(UndergroundExtinctionMode mode)
		{
			underground_extinction_mode_ = mode;
		}
		
		UndergroundExtinctionMode GetUndergroundExtinctionMode() const
		{
			return underground_extinction_mode_;
		}

	private:
		//! airmass computation for @param cosZ = cosine of zenith angle z (=sin(altitude)!).
		//! The default (@param apparent_z = true) is computing airmass from observed altitude, following Rozenberg (1966) [X(90)~40].
		//! if (@param apparent_z = false), we have geometrical altitude and compute airmass from that,
		//! following Young: Air mass and refraction. Applied Optics 33(6), pp.1108-1110, 1994. [X(90)~32].
		//! A problem is that refraction depends on air pressure and temperature, but Young's formula assumes T=15C, p=1013.25mbar.
		//! So, it seems better to compute refraction first, and then use the Rozenberg formula here.
		//! Rozenberg is infinite at Z=92.17 deg, Young at Z=93.6 deg, so this function RETURNS SUBHORIZONTAL_AIRMASS BELOW -2 DEGREES!
		float AirMass(float cos_z, bool apparent_z = true) const;

	private:
		//! k, magnitudes/airmass, in [0.00, ... 1.00], (default 0.20).
		float ext_coeff_ = 0.13f;

		//! Define what we are going to do for underground stars when ground is not rendered
		UndergroundExtinctionMode underground_extinction_mode_ = UndergroundExtinctionMode::Mirror;
	};

	//! @class Refraction
	//! This class performs refraction computations, following literature from atmospheric optics and astronomy.
	//! Refraction solutions can only be approximate, given the turbulent, unpredictable real atmosphere.
	//! Typical horizons do not go down below -1, so strange effects (distortion) between -2 and -5 should be covered.
	//! Note that forward/backward are no absolute reverse operations!
	//! All the computations should be in effect
	//! (1) only if atmosphere effects are true
	//! (2) only for celestial objects, never for landscape images
	//! (3) only for terrestrial locations, not on Moon/Mars/Saturn etc

	class Refraction : public Projector::ModelViewTransform
	{
	public:
		Refraction();

		//! Apply refraction.
		//! @param altAzPos is the geometrical star position vector, to be transformed into apparent position.
		//! Note that forward/backward are no absolute reverse operations!
		void Forward(glm::dvec3& alt_az_pos) const override;

		//! Remove refraction from position ("reduce").
		//! @param altAzPos is the apparent star position vector, to be transformed into geometrical position.
		//! Note that forward/backward are no absolute reverse operations!
		void Backward(glm::dvec3& alt_az_pos) const override;

		void Combine(glm::dmat4 const & m) override
		{
			this->SetPreTransformMatrix(pre_transform_mat_ * m);
		}

		glm::dmat4 GetTransformMatrix() const override;

		std::shared_ptr<Projector::ModelViewTransform> Clone() const override;

		//! Get/Set surface air pressure (mbars), influences refraction computation.
		float GetPressure() const
		{
			return pressure_;
		}
		void SetPressure(float p_mbar);

		//! Get/Set surface air temperature (degrees Celsius), influences refraction computation.
		float GetTemperature() const
		{
			return temperature_;
		}
		void SetTemperature(float t_c);

		//! Set the transformation matrices used to transform input vector to AltAz frame.
		void SetPreTransformMatrix(glm::dmat4 const & m);
		void SetPostTransformMatrix(glm::dmat4 const & m);

	private:
		//! Update precomputed variables.
		void UpdatePrecomputed();

		void InnerRefractionForward(glm::dvec3& alt_az_pos) const;
		void InnerRefractionBackward(glm::dvec3& alt_az_pos) const;

		//! These 3 Atmosphere parameters can be controlled by GUI.
		//! Pressure[mbar]
		float pressure_ = 1013;
		//! Temperature[Celsius deg]
		float temperature_ = 10;
		//! Correction factor for refraction formula, to be cached for speed.
		float press_temp_corr_;

		//! Used to pretransform coordinates into AltAz frame.
		glm::dmat4 pre_transform_mat_;
		glm::dmat4 invert_pre_transform_mat_;

		//! Used to postransform refracted coordinates from AltAz to view.
		glm::dmat4 post_transform_mat_;
		glm::dmat4 invert_post_transform_mat_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_REFRACTION_EXTINCTION_HPP
