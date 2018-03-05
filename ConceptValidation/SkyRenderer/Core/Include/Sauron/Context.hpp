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

#ifndef SAURON_SKY_RENDERER_CORE_CONTEXT_HPP
#define SAURON_SKY_RENDERER_CORE_CONTEXT_HPP

#pragma once

#include <memory>

#include <Sauron/Core.hpp>
#include <Sauron/LocationManager.hpp>

namespace Sauron
{
	//! @class Context
	//! Singleton main Sauron context class.
	class Context
	{
	public:
		//! Create and initialize the main Sauron context.
		Context();

		//! Get the Context singleton instance.
		static Context& GetInstance();

		//! Get the core of Sauron.
		Core& GetCore()
		{
			return core_;
		}

		//! Set system-dependent location manager.
		void SetLocationManager(std::unique_ptr<LocationManager> loc_mgr);

		//! Get location manager to use for managing locations.
		LocationManager& GetLocationManager();

	private:
		// The Context singleton
		static std::unique_ptr<Context> instance_;

		// The associated Core instance
		Core core_;

		// Manager for observer locations
		std::unique_ptr<LocationManager> location_mgr_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_CONTEXT_HPP
