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

#include <Sauron/LocationManager.hpp>

#include <regex>

#include <boost/algorithm/string.hpp>

namespace
{
	float ParseAngle(std::string const & s)
	{
		float ret;
		try
		{
			// First try normal decimal value.
			ret = std::stof(s);
			return ret;
		}
		catch (...)
		{
		};

		// Try GPS coordinate like +121°33'38.28"
		std::regex reg("([+-]?[\\d.]+)°(?:([\\d.]+)')?(?:([\\d.]+)\")?");
		std::smatch match;
		if (std::regex_match(s, match, reg))
		{
			try
			{
				float deg = std::stof(match[1]);
				float min = ((match.size() > 1) && !match[2].str().empty()) ? std::stof(match[2]) : 0;
				float sec = ((match.size() > 2) && !match[3].str().empty()) ? std::stof(match[3]) : 0;
				return deg + min / 60 + sec / 3600;
			}
			catch (...)
			{
			}
		}
		return 0;
	}
}

namespace Sauron
{
	LocationManager::LocationManager()
	{
		last_location_ = this->LocationFromSystem();
	}

	Location LocationManager::LocationForString(std::string const & s) const
	{
		Location ret;
		ret.SetValid(false);

		// A coordinate set, like 47.6801,-122.121
		std::regex reg("(?:(.+)\\s+)?(.+),(.+)");
		std::smatch match;
		if (std::regex_match(s, match, reg))
		{
			try
			{
				ret.SetLatitude(ParseAngle(boost::trim_copy(match[2].str())));
				ret.SetLongitude(ParseAngle(boost::trim_copy(match[3].str())));
				ret.SetValid(true);
			}
			catch (...)
			{
			}
		}
		return ret;
	}

	Location LocationManager::LocationFromSystem()
	{
		// TODO: Get real location
		return this->LocationForString("47.6801,-122.121");
	}
}
