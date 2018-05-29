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

#ifndef SAURON_SKY_RENDERER_CORE_MODULE_HPP
#define SAURON_SKY_RENDERER_CORE_MODULE_HPP

#pragma once

#include <string>

#include <Sauron/Util.hpp>

namespace Sauron
{
	class Core;

	//! This is the common base class for all the main components of Sauron.
	//! Standard Modules are the one displaying the stars, the constellations, the planets etc..
	//! Every new module derived class should implement the methods defined in class Module.
	//! Once a module is registered into the ModuleManager, it is automatically managed by the program.
	//! It can be called using the GETMODULE macro, passing as argument its name, which is also the QObject name
	//! Because StelModules are very generic components, it is also possible to load them dynamically,
	//! thus enabling creation of external plug-ins for stellarium.
	//! @sa StelObjectModule for StelModule managing collections of StelObject.
	//! @sa @ref plugins for documentation on how to develop external plugins.
	//!
	//! There are several signals that StelApp emits that subclasses may be interested in:
	//! laguageChanged()
	//!	Update i18n strings from english names according to current global sky and application language.
	//!	This method also reload the proper fonts depending on the language.
	//!	The translation shall be done using the StelTranslator provided by the StelApp singleton instance.
	//! skyCultureChanged(const QString&)
	//!	Update sky culture, i.e. load data if necessary and translate them to current sky language if needed.
	//! colorSchemeChanged(const QString&)
	//!	Load the given color style
	class Module
	{
	public:
		explicit Module(std::string const & name)
			: name_(name)
		{
		}
		virtual ~Module() = default;

		//! Return the name of this Module
		std::string const & Name() const
		{
			return name_;
		}

		//! Initialize itself.
		//! If the initialization takes significant time, the progress should be displayed on the loading bar.
		virtual void Init() = 0;

		//! Called before the module will be delete, and before the openGL context is suppressed.
		//! Deinitialize all openGL texture in this method.
		virtual void Deinit()
		{
		}

		//! Execute all the drawing functions for this module.
		//! @param core the core to use for the drawing
		virtual void Draw(Core& core)
		{
			SAURON_UNUSED(core);
		}
		
		//! Update the module with respect to the time.
		//! @param deltaTime the time increment in second since last call.
		virtual void Update(double delta_time) = 0;

		//! Define the possible action for which an order is defined
		enum class Action
		{
			Draw,              //!< Action associated to the Draw() method
			Update,            //!< Action associated to the Update() method
		};

		//! Return the value defining the order of call for the given action
		//! For example if stars.callOrder[ActionDraw] == 10 and constellation.callOrder[ActionDraw] == 11,
		//! the stars module will be drawn before the constellations
		//! @param actionName the name of the action for which we want the call order
		//! @return the value defining the order. The closer to 0 the earlier the module's action will be called
		virtual float GetCallOrder(Action action) const
		{
			SAURON_UNUSED(action);
			return 0;
		}

	protected:
		std::string name_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_MODULE_MANAGER_HPP
