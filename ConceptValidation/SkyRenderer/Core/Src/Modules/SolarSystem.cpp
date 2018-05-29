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

#include "SolarSystem.hpp"

#include <iostream>

#include <Sauron/Context.hpp>
#include <Sauron/Core.hpp>
#include <Sauron/ModuleManager.hpp>

#include "Planet.hpp"
#include "../PlanetsEphems/EphemWrapper.hpp"

namespace Sauron
{
	SolarSystem::SolarSystem()
		: Module("SolarSystem")
	{
	}

	SolarSystem::~SolarSystem()
	{
		this->Deinit();
	}

	float SolarSystem::GetCallOrder(Action action) const
	{
		if (action == Module::Action::Draw)
		{
			// TODO
			return /*GET_MODULE(StarMgr)->GetCallOrder(action) + */10;
		}
		return 0;
	}

	void SolarSystem::Init()
	{
		sun_ = std::make_shared<Planet>("Sun", 696000, 0, get_sun_helio_coordsv, nullptr);
		system_planets_.push_back(sun_);

		earth_ = std::make_shared<Planet>("Earth", 6378.1366, 0.003352810664747481, get_earth_helio_coordsv, nullptr);
		earth_->parent_ = sun_;
		system_planets_.push_back(earth_);

		moon_ = std::make_shared<Planet>("Earth", 1737.4, 0, get_lunar_parent_coordsv, nullptr);
		moon_->parent_ = earth_;
		system_planets_.push_back(moon_);

		// Compute position and matrix of sun and all the satellites (ie planets)
		// for the first initialization Q_ASSERT that center is sun center (only impacts on light speed correction)	
		this->ComputePositions(Context::GetInstance().GetCore().GetJDE(), this->GetSun());
	}

	void SolarSystem::Deinit()
	{
	}

	void SolarSystem::Draw(Core& core)
	{
		SAURON_UNUSED(core);
	}

	void SolarSystem::Update(double delta_time)
	{
		SAURON_UNUSED(delta_time);
	}

	// Compute the position for every elements of the solar system.
	// The order is not important since the position is computed relatively to the mother body
	void SolarSystem::ComputePositions(double date_jde, std::shared_ptr<Planet> const & observer_planet)
	{
		for (auto p : system_planets_)
		{
			p->ComputePositionWithoutOrbits(date_jde);
		}
		// BEGIN HACK: 0.16.0post for solar aberration/light time correction
		// This fixes eclipse bug LP:#1275092) and outer planet rendering bug (LP:#1699648) introduced by the first fix in 0.16.0.
		// We compute a "light time corrected position" for the sun and apply it only for rendering, not for other computations.
		// A complete solution should likely "just" implement aberration for all objects.
		auto const obs_pos_jde = observer_planet->GetHeliocentricEclipticPos();
		auto const obs_dist = glm::length(obs_pos_jde);

		observer_planet->ComputePosition(date_jde - obs_dist * (AU / (SPEED_OF_LIGHT * (24 * 60 * 60))));
		glm::dvec3 const obs_pos_jde_before = observer_planet->GetHeliocentricEclipticPos();
		light_time_sun_position_ = obs_pos_jde - obs_pos_jde_before;

		// We must reset observerPlanet for the next step!
		observer_planet->ComputePosition(date_jde);
		// END HACK FOR SOLAR LIGHT TIME/ABERRATION
		for (auto p : system_planets_)
		{
			const double light_speed_correction = glm::length(p->GetHeliocentricEclipticPos() - obs_pos_jde) * (AU / (SPEED_OF_LIGHT * (24 * 60 * 60)));
			p->ComputePosition(date_jde - light_speed_correction);
		}

		this->ComputeTransMatrices(date_jde, observer_planet->GetHeliocentricEclipticPos());
	}

	// Compute the transformation matrix for every elements of the solar system.
	// The elements have to be ordered hierarchically, eg. it's important to compute earth before moon.
	void SolarSystem::ComputeTransMatrices(double date_jde, glm::dvec3 const & observer_pos)
	{
		double date_jd = date_jde - (Context::GetInstance().GetCore().ComputeDeltaT(date_jde)) / (24 * 60 * 60);

		for (auto p : system_planets_)
		{
			const double light_speed_correction = glm::length(p->GetHeliocentricEclipticPos() - observer_pos) * (AU / (SPEED_OF_LIGHT * (24 * 60 * 60)));
			p->ComputeTransMatrix(date_jd - light_speed_correction, date_jde - light_speed_correction);
		}
	}
}


