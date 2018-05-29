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

#ifndef SAURON_SKY_RENDERER_CORE_MODULES_SOLARSYSTEM_HPP
#define SAURON_SKY_RENDERER_CORE_MODULES_SOLARSYSTEM_HPP

#pragma once

#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include <Sauron/Module.hpp>

namespace Sauron
{
	class Planet;

	//! @class SolarSystem
	//! This includes the Major Planets, Minor Planets and Comets.
	class SolarSystem : public Module
	{
	public:
		SolarSystem();
		~SolarSystem() override;

		//! Initialize the SolarSystem.
		//! Includes:
		//! - loading planetary body orbital and model data from data/ssystem.ini
		//! - perform initial planet position calculation
		//! - (TODO) set display options from application settings
		void Init() override;

		void Deinit() override;

		//! Draw SolarSystem objects (planets).
		//! @param core The StelCore object.
		//! @return The maximum squared distance in pixels that any SolarSystem object
		//! has travelled since the last update.
		void Draw(Core& core) override;

		//! Update time-varying components.
		//! This includes planet motion trails.
		void Update(double delta_time) override;

		//! Used to determine what order to draw the various StelModules.
		float GetCallOrder(Action action) const override;

		//! Get the Planet object pointer for the Sun.
		std::shared_ptr<Planet> const & GetSun() const
		{
			return sun_;
		}

		//! Get the Planet object pointer for the Earth.
		std::shared_ptr<Planet> const & GetEarth() const
		{
			return earth_;
		}

		//! Get the Planet object pointer for Earth's moon.
		std::shared_ptr<Planet> const & GetMoon() const
		{
			return moon_;
		}

		//! Compute the position and transform matrix for every element of the solar system.
		//! @param dateJDE the Julian Day in JDE (Ephemeris Time or equivalent)	
		//! @param observerPlanet planet of the observer (Required for light travel time or aberration computation).
		void ComputePositions(double date_jde, std::shared_ptr<Planet> const & observer_planet);

	private:
		//! Compute the transformation matrix for every elements of the solar system.
		//! observerPos is needed for light travel time computation.
		void ComputeTransMatrices(double date_jde, glm::dvec3 const & observer_pos);

	private:
		std::shared_ptr<Planet> sun_;
		std::shared_ptr<Planet> earth_;
		std::shared_ptr<Planet> moon_;

		//! List of all the bodies of the solar system.
		std::vector<std::shared_ptr<Planet>> system_planets_;

		glm::dvec3 light_time_sun_position_;			// when observing a solar eclipse, we need solar position 8 minutes ago.
	};
}

#endif // SAURON_SKY_RENDERER_CORE_MODULES_SOLARSYSTEM_HPP
