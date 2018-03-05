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

namespace Sauron
{
	//! @class Observer
	class Observer
	{
	public:
		//! Create a new Observer instance which is at a fixed Location
		explicit Observer(Location const & loc);

		//! Get the informations on the current location
		Location const & GetCurrentLocation() const
		{
			return curr_location_;
		}

	protected:
		Location curr_location_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_OBSERVER_HPP
