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

#include <Sauron/ErrorHandling.hpp>

#include <iostream>
#include <stdexcept>

namespace Sauron
{
#if defined(KLAYGE_DEBUG) || !defined(KLAYGE_BUILTIN_UNREACHABLE)
	void UnreachableInternal(char const * msg, char const * file, uint32_t line)
	{
		if (msg)
		{
			std::cerr << msg << std::endl;
		}
		if (file)
		{
			std::cerr << "UNREACHABLE executed at " << file << ": " << line << "." << std::endl;
		}
		else
		{
			std::cerr << "UNREACHABLE executed." << std::endl;
		}

		throw std::runtime_error("Unreachable.");
	}
#endif
}
