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

#ifndef SAURON_SKY_RENDERER_CORE_LOCATION_HPP
#define SAURON_SKY_RENDERER_CORE_LOCATION_HPP

#include <string>

namespace Sauron
{
	//! @class Location
	//! Store the informations for a location on a planet
	class Location
	{
	public:
		Location()
		{
		}

		//! Get the longitude in degree.
		float GetLongitude() const
		{
			return longitude_;
		}
		//! Set the longitude in degree.
		void SetLongitude(float longitude)
		{
			longitude_ = longitude;
		}

		//! Get the latitude in degree.
		float GetLatitude() const
		{
			return latitude_;
		}
		//! Set the latitude in degree.
		void SetLatitude(float latitude)
		{
			latitude_ = latitude;
		}

		//! Get the altitude in meter.
		float GetAltitude() const
		{
			return altitude_;
		}
		//! Set the altitude in meter.
		void SetAltitude(float altitude)
		{
			altitude_ = altitude;
		}

		//! Get IANA identificator of time zone.
		std::string const & GetIanaTimeZone() const
		{
			return iana_time_zone_;
		}
		//! Set IANA identificator of time zone.
		void SetIanaTimeZone(std::string const & tz)
		{
			iana_time_zone_ = tz;
		}

		//! Get if the location is valid.
		bool GetValid() const
		{
			return valid_;
		}
		//! Set if the location is valid.
		void SetValid(bool v)
		{
			valid_ = v;
		}

	private:
		float longitude_ = 0;
		float latitude_ = 0;
		float altitude_ = 0;

		std::string iana_time_zone_;

		bool valid_ = true;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_LOCATION_HPP
