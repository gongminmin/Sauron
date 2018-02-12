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

#ifndef SAURON_SKY_RENDERER_CORE_LOCATION_MANAGER_HPP
#define SAURON_SKY_RENDERER_CORE_LOCATION_MANAGER_HPP

#include <Sauron/Location.hpp>

#include <string>

namespace Sauron
{
	//! @class LocationManager
	//! Retrieve and manage the locations.
	class LocationManager
	{
	public:
		LocationManager();

		//! Return a valid location when no valid one was found.
		Location const & GetLastLocation() const
		{
			return last_location_;
		}

		//! Return the location for a given string can match coordinates.
		Location LocationForString(std::string const & s) const;

		//! Find location via host system.
		void LocationFromSystem();

	private:
		Location last_location_;
	};
}

#endif // SAURON_SKY_RENDERER_CORE_LOCATION_MANAGER_HPP
