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

#include <Sauron/ModuleManager.hpp>

#include <algorithm>
#include <iostream>

namespace Sauron
{
	ModuleManager::ModuleManager()
	{
		call_orders_.emplace(Module::Action::Draw, std::vector<Module*>());
		call_orders_.emplace(Module::Action::Update, std::vector<Module*>());
	}

	void ModuleManager::Update()
	{
		if (regenerate_calling_lists_)
		{
			this->GenerateCallingLists();
		}
		regenerate_calling_lists_ = false;
	}

	void ModuleManager::RegisterModule(std::unique_ptr<Module> m, bool generate_calling_lists)
	{
		auto const & name = m->Name();
		if (modules_.find(name) != modules_.end())
		{
			std::cout << "Module" << name << "is already loaded.";
		}
		else
		{
			modules_.emplace(name, std::move(m));

			if (generate_calling_lists)
			{
				this->GenerateCallingLists();
			}
		}
	}

	void ModuleManager::UnloadModule(std::string const & module_name)
	{
		auto iter = modules_.find(module_name);
		if (iter == modules_.end())
		{
			std::cout << "Module" << module_name << "is not loaded.";
			return;
		}

		modules_.erase(iter);

		regenerate_calling_lists_ = true;
	}

	Module* ModuleManager::GetModule(std::string const & module_name, bool no_warning)
	{
		Module* ret = nullptr;

		auto iter = modules_.find(module_name);
		if (iter == modules_.end())
		{
			if (!no_warning)
			{
				std::cout << "Unable to find module called " << module_name;
			}
		}
		else
		{
			ret = iter->second.get();
		}

		return ret;
	}
	
	void ModuleManager::GenerateCallingLists()
	{
		for (auto iter = call_orders_.begin(); iter != call_orders_.end(); ++ iter)
		{
			std::vector<Module*> tmp_modules;
			for (auto const & mod : modules_)
			{
				tmp_modules.push_back(mod.second.get());
			}
			auto action = iter->first;
			std::sort(tmp_modules.begin(), tmp_modules.end(), [&action](Module* lhs, Module* rhs)
				{
					return lhs->GetCallOrder(action) < rhs->GetCallOrder(action);
				});

			iter->second.swap(tmp_modules);
		}
	}
}
