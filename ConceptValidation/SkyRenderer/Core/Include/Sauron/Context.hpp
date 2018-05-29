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
#include <Sauron/GraphicsSystem.hpp>
#include <Sauron/LocationManager.hpp>
#include <Sauron/ModuleManager.hpp>

namespace Sauron
{
	//! @class Context
	//! Singleton main Sauron context class.
	class Context
	{
	public:
		//! Create and initialize the main Sauron context.
		Context();
		~Context();

		//! Get the Context singleton instance.
		static Context& GetInstance();

		//! Initialize core and all the modules.
		void Init(void* wnd);

		//! Deinitialize core and all the modules.
		void Deinit();

		//! Get the graphics system of Sauron.
		GraphicsSystem& GetGraphicsSystem()
		{
			return *graphics_sys_;
		}

		//! Get the core of Sauron.
		Core& GetCore()
		{
			return *core_;
		}

		//! Set system-dependent location manager.
		void SetLocationManager(std::unique_ptr<LocationManager> loc_mgr);

		//! Get location manager to use for managing locations.
		LocationManager& GetLocationManager();

		//! Get the module manager to use for accessing any module loaded in the context.
		ModuleManager& GetModuleManager();

		//! Update all object according to the deltaTime in seconds.
		void Update(double delta_time);

		//! Draw all registered StelModule in the order defined by the order lists.
		// 2014-11: OLD COMMENT? What does a void return?
		// @return the max squared distance in pixels that any object has travelled since the last update.
		void Draw();

	private:
		bool initialized_ = false;

		// The Context singleton
		static std::unique_ptr<Context> instance_;

		// The associated GraphicsSystem instance
		std::unique_ptr<GraphicsSystem> graphics_sys_;

		// The associated Core instance
		std::unique_ptr<Core> core_;

		// Module manager for the context
		std::unique_ptr<ModuleManager> module_mgr_;

		// Locale manager for the context
		std::unique_ptr<LocationManager> location_mgr_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_CONTEXT_HPP
