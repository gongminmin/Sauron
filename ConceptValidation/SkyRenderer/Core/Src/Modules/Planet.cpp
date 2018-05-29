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

#include "Planet.hpp"

#include <boost/assert.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <Sauron/Context.hpp>
#include <Sauron/Util.hpp>

#include "../PlanetsEphems/precession.h"
#include "../PlanetsEphems/sidereal_time.h"

namespace Sauron
{
	Planet::Planet(std::string const & english_name,
			double radius,
			double oblateness,
			PosFunc coord_func,
			void* orbit_ptr)
		: english_name_(english_name),
			radius_(radius),
			one_minus_oblateness_(1 - oblateness),
			coord_func_(coord_func),
			orbit_ptr_(orbit_ptr)
	{
	}

	// Compute the z rotation to use from equatorial to geographic coordinates.
	// We need both JD and JDE here for Earth. (For other planets only JDE.)
	double Planet::GetSiderealTime(double jd, double jde) const
	{
		if (english_name_ == "Earth")
		{	// Check to make sure that nutation is just those few arcseconds.
			if (Context::GetInstance().GetCore().GetUseNutation())
			{
				return get_apparent_sidereal_time(jd, jde);
			}
			else
			{
				return get_mean_sidereal_time(jd, jde);
			}
		}

		double t = jde - re_.epoch;
		// oops... avoid division by zero (typical case for moons with chaotic period of rotation)
		double rotations = 1; // NOTE: Maybe 1e-3 will be better?
		if (re_.period != 0) // OK, it's not a moon with chaotic period of rotation :)
		{
			rotations = t / re_.period;
		}
		double whole_rotations = std::floor(rotations);
		double remainder = rotations - whole_rotations;

		if (english_name_ == "Jupiter")
		{
			// http://www.projectpluto.com/grs_form.htm
			// CM( System II) =  181.62 + 870.1869147 * jd + correction [870d rotation every day]
			double jup_mean = (jde - 2455636.938) * 360.0 / 4332.89709;
			double eqn_center = 5.55 * std::sin(Deg2Rad(jup_mean));
			double angle = (jde - 2451870.628) * 360.0 / 398.884 - eqn_center;
			//double correction = 11 * sin(Deg2Rad(angle)) + 5 * cos(Deg2Rad(angle))- 1.25 * cos(Deg2Rad(jup_mean)) - eqn_center; // original correction
			double correction = 25.8 + 11 * std::sin(Deg2Rad(angle)) - 2.5 * cos(Deg2Rad(jup_mean)) - eqn_center; // light speed correction not used because in stellarium the jd is manipulated for that
			double cm2 = 181.62 + 870.1869147 * jde + correction; // Central Meridian II
			cm2 = cm2 - 360.0 * static_cast<int>(cm2 / 360.0);
			// http://www.skyandtelescope.com/observing/transit-times-of-jupiters-great-red-spot/ writes:
			// The predictions assume the Red Spot was at Jovian System II longitude 216бу in September 2014 and continues to drift 1.25бу per month, based on historical trends noted by JUPOS.
			// GRS longitude was at 2014-09-08 216d with a drift of 1.25d every month
			double longitude_grs = 216 + 1.25 * (jde - 2456908) / 30;
			// qDebug() << "Jupiter: CM2 = " << cm2 << " longitudeGRS = " << longitudeGRS << " --> rotation = " << (cm2 - longitudeGRS);
			return cm2 - longitude_grs + 50.0;	// Jupiter Texture not 0d
												// To verify:
												// GRS at 2015-02-26 23:07 UT on picture at https://maximusphotography.files.wordpress.com/2015/03/jupiter-febr-26-2015.jpg
												//        2014-02-25 19:03 UT    http://www.damianpeach.com/jup1314/2014_02_25rgb0305.jpg
												//	  2013-05-01 10:29 UT    http://astro.christone.net/jupiter/jupiter2012/jupiter20130501.jpg
												//        2012-10-26 00:12 UT at http://www.lunar-captures.com//jupiter2012_files/121026_JupiterGRS_Tar.jpg
												//	  2011-08-28 02:29 UT at http://www.damianpeach.com/jup1112/2011_08_28rgb.jpg
												// stellarium 2h too early: 2010-09-21 23:37 UT http://www.digitalsky.org.uk/Jupiter/2010-09-21_23-37-30_R-G-B_800.jpg
		}
		else
		{
			return remainder * 360. + re_.offset;
		}
	}

	glm::dmat4 Planet::GetRotEquatorialToVsop87() const
	{
		glm::dmat4 ret = rot_local_to_parent_;
		if (parent_)
		{
			for (auto p = parent_.get(); p->parent_.get(); p = p->parent_.get())
			{
				ret = p->rot_local_to_parent_ * ret;
			}
		}
		return ret;
	}

	void Planet::SetRotEquatorialToVsop87(glm::dmat4 const & m)
	{
		glm::dmat4 accu(1);
		if (parent_)
		{
			for (auto p = parent_.get(); p->parent_.get(); p = p->parent_.get())
			{
				accu = p->rot_local_to_parent_ * accu;
			}
		}
		rot_local_to_parent_ = glm::transpose(accu) * m;
	}

	void Planet::SetRotationElements(float period, float offset, double epoch, float obliquity, float ascending_node,
		float precession_rate, double sidereal_period)
	{
		re_.period = period;
		re_.offset = offset;
		re_.epoch = epoch;
		re_.obliquity = obliquity;
		re_.ascending_node = ascending_node;
		re_.precession_rate = precession_rate;
		re_.sidereal_period = sidereal_period;  // used for drawing orbit lines

		delta_orbit_jde_ = re_.sidereal_period / ORBIT_SEGMENTS;
	}

	// Return heliocentric coordinate of p
	glm::dvec3 Planet::GetHeliocentricPos(glm::dvec3 const & p) const
	{
		glm::dvec3 pos = p;
		auto const * parent = parent_.get();
		if (parent)
		{
			while (parent->GetParent().get())
			{
				pos += parent->ecliptic_pos_;
				parent = parent->GetParent().get();
			}
		}
		return pos;
	}

	void Planet::SetHeliocentricEclipticPos(glm::dvec3 const & pos)
	{
		ecliptic_pos_ = pos;
		auto parent = parent_.get();
		if (parent)
		{
			while (parent->GetParent().get())
			{
				ecliptic_pos_ -= parent->ecliptic_pos_;
				parent = parent->GetParent().get();
			}
		}
	}

	// Compute the position in the parent Planet coordinate system
	// Actually call the provided function to compute the ecliptical position
	void Planet::ComputePositionWithoutOrbits(double date_jde)
	{
		if (std::abs(last_jde_ - date_jde) > delta_jde_)
		{
			coord_func_(date_jde, &ecliptic_pos_.x, &ecliptic_velocity_.x, orbit_ptr_);
			last_jde_ = date_jde;
		}
	}

	void Planet::ComputePosition(double date_jde)
	{
		// Make sure the parent position is computed for the date_jde, otherwise
		// getHeliocentricPos() would return incorrect values.
		if (parent_)
		{
			parent_->ComputePositionWithoutOrbits(date_jde);
		}

		if (fabs(last_jde_ - date_jde) > delta_jde_)
		{
			// Calculate actual Planet position
			coord_func_(date_jde, &ecliptic_pos_.x, &ecliptic_velocity_.x, orbit_ptr_);
			last_jde_ = date_jde;
		}
	}

	// Compute the transformation matrix from the local Planet coordinate system to the parent Planet coordinate system.
	// In case of the planets, this makes the axis point to their respective celestial poles.
	// TODO: Verify for the other planets if their axes are relative to J2000 ecliptic (VSOP87A XY plane) or relative to (precessed) ecliptic of date?
	void Planet::ComputeTransMatrix(double jd, double jde)
	{
		// We have to call with both to correct this for earth with the new model.
		axis_rotation_ = static_cast<float>(this->GetSiderealTime(jd, jde));

		// Special case - heliocentric coordinates are relative to eclipticJ2000 (VSOP87A XY plane),
		// not solar equator...

		if (parent_)
		{
			// We can inject a proper precession plus even nutation matrix in this stage, if available.
			if (english_name_ == "Earth")
			{
				// rotLocalToParent = Mat4d::zrotation(re.ascendingNode - re.precessionRate*(jd-re.epoch)) * Mat4d::xrotation(-getRotObliquity(jd));
				// We follow Capitaine's (2003) formulation P=Rz(Chi_A)*Rx(-omega_A)*Rz(-psi_A)*Rx(eps_o).
				// ADS: 2011A&A...534A..22V = A&A 534, A22 (2011): Vondrak, Capitane, Wallace: New Precession Expressions, valid for long time intervals:
				// See also Hilton et al., Report on Precession and the Ecliptic. Cel.Mech.Dyn.Astr. 94:351-367 (2006), eqn (6) and (21).
				double eps_a, chi_a, omega_a, psi_a;
				getPrecessionAnglesVondrak(jde, &eps_a, &chi_a, &omega_a, &psi_a);
				// Canonical precession rotations: Nodal rotation psi_A,
				// then rotation by omega_A, the angle between EclPoleJ2000 and EarthPoleOfDate.
				// The final rotation by chi_A rotates the equinox (zero degree).
				// To achieve ecliptical coords of date, you just have now to add a rotX by epsilon_A (obliquity of date).

				rot_local_to_parent_ = glm::rotate(glm::dmat4(1), -psi_a, glm::dvec3(0, 0, 1))
					* glm::rotate(glm::dmat4(1), -omega_a, glm::dvec3(1, 0, 0))
					* glm::rotate(glm::dmat4(1), chi_a, glm::dvec3(0, 0, 1));
				// Plus nutation IAU-2000B:
				if (Context::GetInstance().GetCore().GetUseNutation())
				{
					double delta_psi, delta_eps;
					getNutationAngles(jde, &delta_psi, &delta_eps);
					glm::dmat4 nut2000b = glm::rotate(glm::dmat4(1), eps_a, glm::dvec3(1, 0, 0))
						* glm::rotate(glm::dmat4(1), delta_psi, glm::dvec3(0, 0, 1))
						* glm::rotate(glm::dmat4(1), -eps_a - delta_eps, glm::dvec3(1, 0, 0));
					rot_local_to_parent_ = rot_local_to_parent_ * nut2000b;
				}
			}
			else
			{
				rot_local_to_parent_ = glm::rotate(glm::dmat4(1), re_.ascending_node - re_.precession_rate * (jde - re_.epoch), glm::dvec3(0, 0, 1))
					* glm::rotate(glm::dmat4(1), static_cast<double>(re_.obliquity), glm::dvec3(1, 0, 0));
			}
		}
	}
}
