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

#include <Sauron/Observer.hpp>

#include <boost/assert.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <Sauron/Context.hpp>
#include <Sauron/ModuleManager.hpp>
#include <Sauron/Util.hpp>

#include "Modules/Planet.hpp"
#include "Modules/SolarSystem.hpp"

namespace Sauron
{
	Observer::Observer(Location const & loc)
		: curr_location_(loc)
	{
		auto* sol_system = GET_MODULE(SolarSystem);
		planet_ = sol_system->GetEarth();
	}

	std::shared_ptr<Planet> const & Observer::GetHomePlanet() const
	{
		BOOST_ASSERT(planet_);
		return planet_;
	}

	glm::dvec3 Observer::GetCenterVsop87Pos() const
	{
		return this->GetHomePlanet()->GetHeliocentricEclipticPos();
	}

	// Used to approximate solution with assuming a spherical planet.
	// we follow Meeus, Astr. Alg. 2nd ed, Ch.11., but used offset rho in a wrong way. (offset angle phi in distance rho.)
	double Observer::GetDistanceFromCenter() const
	{
		double const a = this->GetHomePlanet()->GetRadius();
		double const b_by_a = this->GetHomePlanet()->GetOneMinusOblateness(); // b/a;

		if (std::abs(curr_location_.GetLatitude()) >= 89.9f) // avoid tan(90) issues.
		{
			return a * b_by_a;
		}

		double const lat_rad = Deg2Rad(curr_location_.GetLatitude());
		double const u = std::atan(b_by_a * std::tan(lat_rad));
		// std::cerr << "GetDistanceFromCenter: a=" << a * AU << "b/a=" << b_by_a << "b=" << b_by_a * a * AU  << "lat_rad=" << lat_rad << "u=" << u;
		BOOST_ASSERT(std::abs(u) <= std::abs(lat_rad));
		double const alt_fix = curr_location_.GetAltitude() / (1000.0f * AU * a);

		double const rho_sin_phi_prime = b_by_a * std::sin(u) + alt_fix * std::sin(lat_rad);
		double const rho_cos_phi_prime = std::cos(u) + alt_fix * std::cos(lat_rad);

		double const rho = sqrt(rho_sin_phi_prime * rho_sin_phi_prime + rho_cos_phi_prime * rho_cos_phi_prime);
		return rho * a;
	}

	// Used to approximate solution with assuming a spherical planet.
	glm::dvec3 Observer::GetTopographicOffsetFromCenter() const
	{
		double const a = this->GetHomePlanet()->GetRadius();
		double const b_by_a = this->GetHomePlanet()->GetOneMinusOblateness(); // b/a;

		if (std::abs(curr_location_.GetLatitude()) >= 89.9f) // avoid tan(90) issues.
		{
			return glm::dvec3(a * b_by_a);
		}

		double const lat_rad = Deg2Rad(curr_location_.GetLatitude());
		double const u = std::atan(b_by_a * std::tan(lat_rad));
		// std::cerr << "GetDistanceFromCenter: a=" << a * AU << "b/a=" << b_by_a << "b=" << b_by_a * a * AU  << "lat_rad=" << lat_rad << "u=" << u;
		BOOST_ASSERT(std::abs(u) <= std::abs(lat_rad));
		double const alt_fix = curr_location_.GetAltitude() / (1000.0f * AU * a);

		double const rho_sin_phi_prime = b_by_a * std::sin(u) + alt_fix * std::sin(lat_rad);
		double const rho_cos_phi_prime = std::cos(u) + alt_fix * std::cos(lat_rad);

		double const rho = sqrt(rho_sin_phi_prime * rho_sin_phi_prime + rho_cos_phi_prime * rho_cos_phi_prime);
		double const phi_prime = asin(rho_sin_phi_prime / rho);
		return glm::dvec3(rho_cos_phi_prime * a, rho_sin_phi_prime * a, phi_prime);
	}

	// For Earth we require JD, for other planets JDE to describe rotation!
	glm::dmat4 Observer::GetRotAltAzToEquatorial(double jd, double jde) const
	{
		double lat = curr_location_.GetLatitude();
		// TODO: Figure out how to keep continuity in sky as we reach poles
		// otherwise sky jumps in rotation when reach poles in equatorial mode
		// This is a kludge
		// GZ: Actually, why would that be? Lat should be clamped elsewhere. Added tests to track down problems in other locations.
		BOOST_ASSERT(lat <= 90);
		BOOST_ASSERT(lat >= -90);
		if (lat > 90)
		{
			lat = 90;
		}
		if (lat < -90)
		{
			lat = -90;
		}
		return glm::rotate(glm::dmat4(1),
				Deg2Rad(this->GetHomePlanet()->GetSiderealTime(jd, jde) + curr_location_.GetLongitude()), glm::dvec3(0, 0, 1))
			* glm::rotate(glm::dmat4(1), Deg2Rad(90.0 - lat), glm::dvec3(0, 1, 0));
	}

	glm::dmat4 Observer::GetRotEquatorialToVsop87() const
	{
		return this->GetHomePlanet()->GetRotEquatorialToVsop87();
	}
}
