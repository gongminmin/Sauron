/*
 * Stellarium
 * Copyright (C) 2010 Fabien Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 *
 * Refraction and extinction computations.
 * Principal implementation: 2010-03-23 GZ=Georg Zotti, Georg.Zotti@univie.ac.at
 */

#include <Sauron/RefractionExtinction.hpp>
#include <Sauron/Context.hpp>

#include <algorithm>

#include <boost/assert.hpp>

namespace Sauron
{
	//! Compute extinction effect for arrays of size num position vectors and magnitudes.
	//! @param alt_az_pos are the NORMALIZED (!!) (geometrical) star position vectors, and their z components sin(geometric_altitude).
	//! This call must therefore be done before application of Refraction if atmospheric effects are on.
	//! Note that forward/backward are no absolute reverse operations!
	void Extinction::Forward(glm::dvec3 const & alt_az_pos, float* mag) const
	{
		BOOST_ASSERT(std::abs(glm::length(alt_az_pos) - 1) < 0.001);
		*mag += this->AirMass(static_cast<float>(alt_az_pos.z), false) * ext_coeff_;
	}

	//! Compute inverse extinction effect for arrays of size num position vectors and magnitudes.
	//! @param alt_az_pos are the NORMALIZED (!!) (geometrical) star position vectors, and their z components sin(geometric_altitude).
	//! Note that forward/backward are no absolute reverse operations!
	void Extinction::Backward(glm::dvec3 const & alt_az_pos, float* mag) const
	{
		*mag -= this->AirMass(static_cast<float>(alt_az_pos.z), false) * ext_coeff_;
	}

	// airmass computation for cosine of zenith angle z
	float Extinction::AirMass(float cos_z, bool apparent_z) const
	{
		if (cos_z < -0.035f) // about -2 degrees. Here, RozenbergZ>574 and climbs fast!
		{
			switch (underground_extinction_mode_)
			{
			case UndergroundExtinctionMode::Zero:
				return 0;

			case UndergroundExtinctionMode::Max:
				return 42;

			case UndergroundExtinctionMode::Mirror:
				cos_z = std::min(1.0f, -0.035f - (cos_z + 0.035f));
			}
		}

		if (apparent_z)
		{
			// Rozenberg 1966, reported by Schaefer (1993-2000).
			return 1.0f / (cos_z + 0.025f * std::exp(-11 * cos_z));
		}
		else
		{
			// Young 1994
			float const nom = (1.002432f * cos_z + 0.148386f) * cos_z + 0.0096467f;
			float const denum = ((cos_z + 0.149864f) * cos_z + 0.0102963f) * cos_z + 0.000303978f;
			return nom / denum;
		}
	}

	/* ***************************************************************************************************** */

	// The following 4 are to be configured, the rest is derived.
	// Recommendations: -4.9/-4.3/0.1/0.1: sharp but continuous transition, no effects below -5.
	//                  -4.3/-4.3/0.7/0.7: sharp but continuous transition, no effects below -5. Maybe better for picking?
	//                  -3/-3/2/2: "strange" zone 2 degrees wide. Both formulae are close near -3. Actually, refraction formulae dubious below 0
	//                   0/0/1/1: "strange zone 1 degree wide just below horizon, no effect below -1. Actually, refraction formulae dubious below 0! But it looks stupid, see the sun!
	//                  Optimum:-3.54/-3.21783/1.46/1.78217. Here forward/backward are almost perfect inverses (-->good picking!), and nothing happens below -5 degrees.
	// This must be -5 or higher.
	static float constexpr MIN_GEO_ALTITUDE_DEG = -3.54f;
	// this must be -4.3 or higher. -3.21783 is an optimal value to fit against forward refraction!
	static float constexpr MIN_APP_ALTITUDE_DEG = -3.21783f;
	// this must be positive. Transition zone goes that far below the values just specified.
	static float constexpr TRANSITION_WIDTH_GEO_DEG = 1.46f;
	static float constexpr TRANSITION_WIDTH_APP_DEG = 1.78217f;

	Refraction::Refraction()
	{
		this->UpdatePrecomputed();
	}

	void Refraction::SetPreTransformMatrix(glm::dmat4 const & m)
	{
		pre_transform_mat_ = m;
		invert_pre_transform_mat_ = glm::inverse(m);
	}

	void Refraction::SetPostTransformMatrix(glm::dmat4 const & m)
	{
		post_transform_mat_ = m;
		invert_post_transform_mat_ = glm::inverse(m);
	}

	void Refraction::UpdatePrecomputed()
	{
		press_temp_corr_ = pressure_ / 1010 * 283 / (273 + temperature_) / 60;
	}

	void Refraction::InnerRefractionForward(glm::dvec3& alt_az_pos) const
	{
		double const length = glm::length(alt_az_pos);
		if (length == 0)
		{
			// Under some circumstances there are zero coordinates. Just leave them alone.
			//std::cerr << "Refraction::innerRefractionForward(): Zero vector detected - Continue with zero vector.";
			return;
		}

		BOOST_ASSERT(length > 0);
		double const sin_geo = alt_az_pos[2] / length;
		BOOST_ASSERT(std::abs(sin_geo) <= 1);
		double geom_alt_rad = std::asin(sin_geo);
		double geom_alt_deg = Rad2Deg(geom_alt_rad);
		if (geom_alt_deg > MIN_GEO_ALTITUDE_DEG)
		{
			// refraction from Saemundsson, S&T1986 p70 / in Meeus, Astr.Alg.
			double const r = press_temp_corr_
				* (1.02 / std::tan((geom_alt_deg + 10.3 / (geom_alt_deg + 5.11)) * M_PI / 180) + 0.0019279);
			geom_alt_deg += r;
			if (geom_alt_deg > 90)
			{
				geom_alt_deg = 90;
			}
		}
		else if (geom_alt_deg > MIN_GEO_ALTITUDE_DEG - TRANSITION_WIDTH_GEO_DEG)
		{
			// Avoids the jump below -5 by interpolating linearly between MIN_GEO_ALTITUDE_DEG and bottom of transition zone
			double const r_m5 = press_temp_corr_
				* (1.02 / std::tan((MIN_GEO_ALTITUDE_DEG + 10.3 / (MIN_GEO_ALTITUDE_DEG + 5.11)) * M_PI / 180) + 0.0019279);
			geom_alt_deg += r_m5 * (geom_alt_deg - (MIN_GEO_ALTITUDE_DEG - TRANSITION_WIDTH_GEO_DEG)) / TRANSITION_WIDTH_GEO_DEG;
		}
		else
		{
			return;
		}

		// At this point we have corrected geometric altitude. Note that if we just change alt_az_pos[2], we would change vector length, so this would change our angles.
		// We have to shorten X,Y components of the vector as well by the change in cosines of altitude, or (sqrt(1-sin(alt))

		double const refr_alt_rad = Deg2Rad(geom_alt_deg);
		double const sin_ref = std::sin(refr_alt_rad);
		double const shorten_xy = ((std::abs(sin_geo) >= 1) ? 1 : std::sqrt((1 - sin_ref * sin_ref) / (1 - sin_geo * sin_geo)));

		alt_az_pos.x *= shorten_xy;
		alt_az_pos.y *= shorten_xy;
		alt_az_pos.z = sin_ref * length;
	}

	// going from observed position to geometrical position.
	void Refraction::InnerRefractionBackward(glm::dvec3& alt_az_pos) const
	{
		double const length = glm::length(alt_az_pos);
		if (length == 0)
		{
			// Under some circumstances there are zero coordinates. Just leave them alone.
			//std::cerr << "Refraction::innerRefractionBackward(): Zero vector detected - Continue with zero vector.";
			return;
		}

		BOOST_ASSERT(length > 0);
		double const sin_obs = alt_az_pos[2] / length;
		BOOST_ASSERT(fabs(sin_obs) <= 1);
		double obs_alt_deg = Rad2Deg(std::asin(sin_obs));
		if (obs_alt_deg > 0.22879)
		{
			// refraction from Bennett, in Meeus, Astr.Alg.
			double const r = press_temp_corr_ * (1 / std::tan((obs_alt_deg + 7.31 / (obs_alt_deg + 4.4)) * M_PI / 180) + 0.0013515);
			obs_alt_deg -= r;
		}
		else if (obs_alt_deg > MIN_APP_ALTITUDE_DEG)
		{
			// backward refraction from polynomial fit against Saemundson[-5...-0.3]
			double const r = (((((0.0444 * obs_alt_deg + 0.7662) * obs_alt_deg + 4.9746) * obs_alt_deg + 13.599)
				* obs_alt_deg + 8.052) * obs_alt_deg - 11.308) * obs_alt_deg + 34.341;
			obs_alt_deg -= press_temp_corr_ * r;
		}
		else if (obs_alt_deg > MIN_APP_ALTITUDE_DEG - TRANSITION_WIDTH_APP_DEG)
		{
			// Compute top value from polynome, apply linear interpolation
			static double constexpr r_min = (((((0.0444 * MIN_APP_ALTITUDE_DEG + 0.7662) * MIN_APP_ALTITUDE_DEG
				+ 4.9746) * MIN_APP_ALTITUDE_DEG + 13.599) * MIN_APP_ALTITUDE_DEG
				+ 8.052) * MIN_APP_ALTITUDE_DEG - 11.308) * MIN_APP_ALTITUDE_DEG + 34.341;

			obs_alt_deg -= r_min * press_temp_corr_
				* (obs_alt_deg - (MIN_APP_ALTITUDE_DEG - TRANSITION_WIDTH_APP_DEG)) / TRANSITION_WIDTH_APP_DEG;
		}
		else
		{
			return;
		}

		// At this point we have corrected observed altitude. Note that if we just change alt_az_pos[2], we would change vector length, so this would change our angles.
		// We have to make X,Y components of the vector a bit longer as well by the change in cosines of altitude, or (sqrt(1-sin(alt))

		double const geo_alt_rad = Deg2Rad(obs_alt_deg);
		double const sin_geo = std::sin(geo_alt_rad);
		double const longer_xy = ((std::abs(sin_obs) >= 1.0) ? 1 : std::sqrt((1 - sin_geo * sin_geo) / (1 - sin_obs * sin_obs)));

		alt_az_pos.x *= longer_xy;
		alt_az_pos.y *= longer_xy;
		alt_az_pos.z = sin_geo * length;
	}

	void Refraction::Forward(glm::dvec3& alt_az_pos) const
	{
		glm::dvec4 v4 = pre_transform_mat_ * glm::dvec4(alt_az_pos, 1);
		alt_az_pos = glm::dvec3(v4.x, v4.y, v4.z);
		this->InnerRefractionForward(alt_az_pos);
		v4 = post_transform_mat_ * glm::dvec4(alt_az_pos, 1);
		alt_az_pos = glm::dvec3(v4.x, v4.y, v4.z);
	}

	//Bennett's formula is not a strict inverse of Saemundsson's. There is a notable discrepancy (alt!=backward(forward(alt))) for
	// geometric altitudes <-.3deg.  This is not a problem in real life, but if a user switches off landscape, click-identify may pose a problem.
	// Below this altitude, we therefore use a polynomial fit that should represent a close inverse of Saemundsson.
	void Refraction::Backward(glm::dvec3& alt_az_pos) const
	{
		glm::dvec4 v4 = invert_post_transform_mat_ * glm::dvec4(alt_az_pos, 1);
		alt_az_pos = glm::dvec3(v4.x, v4.y, v4.z);
		this->InnerRefractionBackward(alt_az_pos);
		v4 = invert_pre_transform_mat_ * glm::dvec4(alt_az_pos, 1);
		alt_az_pos = glm::dvec3(v4.x, v4.y, v4.z);
	}

	glm::dmat4 Refraction::GetTransformMatrix() const
	{
		return post_transform_mat_ * pre_transform_mat_;
	}

	std::shared_ptr<Projector::ModelViewTransform> Refraction::Clone() const
	{
		auto ret = std::make_shared<Refraction>();
		*ret = *this;
		return ret;
	}

	void Refraction::SetPressure(float p)
	{
		pressure_ = p;
		this->UpdatePrecomputed();
	}

	void Refraction::SetTemperature(float t)
	{
		temperature_ = t;
		this->UpdatePrecomputed();
	}
}
