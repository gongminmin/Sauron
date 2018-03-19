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

namespace Sauron
{
	class Location;
	class Observer;
	class Projector;

	//! @class Core
	//! Main class for Sauron sky renderer core processing.
	class Core
	{
	public:
		Core();

		//! Update all the objects with respect to the time.
		//! @param delta_time the time increment in sec.
		void Update(double delta_time);

		//! Render the sky
		void Render();

		//! Replaces the current observer.
		void SetCurrentObserver(std::shared_ptr<Observer> const & ob);

		//! Returns the current observer.
		std::shared_ptr<Observer> const & GetCurrentObserver() const;

		//! Move the observer to the given location
		//! @param target the target location
		void MoveObserverTo(Location const & target);

		//! Replaces the current projector.
		void SetCurrentProjector(std::shared_ptr<Projector> const & proj);

		//! Returns the current projector.
		std::shared_ptr<Projector> const & GetCurrentProjector() const;

	private:
		std::shared_ptr<Observer> curr_observer_;
		std::shared_ptr<Projector> curr_projector_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_LOCATION_HPP
