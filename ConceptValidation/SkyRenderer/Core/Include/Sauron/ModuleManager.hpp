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

#ifndef SAURON_SKY_RENDERER_CORE_MODULE_MANAGER_HPP
#define SAURON_SKY_RENDERER_CORE_MODULE_MANAGER_HPP

#pragma once

#include <Sauron/Module.hpp>

#include <map>
#include <string>
#include <vector>

namespace Sauron
{
	//! @def GET_MODULE(m)
	//! Return a pointer on a Module from its type @a m
	#define GET_MODULE(m) (checked_cast<m*>(Context::GetInstance().GetModuleManager().GetModule(#m)))

	//! @class ModuleManager
	//! Manage a collection of Modules including both core and plugin modules.
	//! The order in which some actions like draw or update are called for each module can be retrieved with the GetCallOrders() method.
	class ModuleManager
	{
	public:
		ModuleManager();
		virtual ~ModuleManager() = default;

		//! Regenerate calling lists if necessary
		void Update();

		//! Register a new Module to the list
		//! The module is later referenced by its QObject name.
		void RegisterModule(std::unique_ptr<Module> m, bool generate_calling_lists = false);

		//! Unregister and delete a Module. The program will hang if other modules depend on the removed one
		//! @param module_name the unique ID of the module
		void UnloadModule(std::string const & module_name);

		//! Get the corresponding module or nullptr if can't find it.
		//! @param module_id the name of the module instance.
		//! @param no_warning if true, don't display any warning if the module is not found.
		Module* GetModule(std::string const & module_name, bool no_warning = false);

		//! Get the list of modules in the correct order for calling the given action
		std::vector<Module*> const & GetCallOrders(Module::Action action)
		{
			return call_orders_[action];
		}

	private:
		//! Generate properly sorted calling lists for each action (e,g, draw, update)
		//! according to modules orders dependencies
		void GenerateCallingLists();

	private:
		//! The main module list associating hash(name):pointer
		std::map<std::string, std::unique_ptr<Module>> modules_;

		//! The list of all module in the correct order for each action
		std::map<Module::Action, std::vector<Module*>> call_orders_;

		//! True if modules were removed, and therefore the calling list need to be regenerated
		bool regenerate_calling_lists_ = true;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_MODULE_MANAGER_HPP
