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

#include <Sauron/Context.hpp>

#include <mutex>

#include <boost/assert.hpp>

#include <Sauron/Module.hpp>
#include <Sauron/Painter.hpp>

#include "Modules/MilkyWay.hpp"
#include "Modules/SolarSystem.hpp"

namespace
{
	std::mutex singleton_mutex;
}

namespace Sauron
{
	std::unique_ptr<Context> Context::instance_;

	Context::Context()
	{
		module_mgr_ = std::make_unique<ModuleManager>();
	}

	Context::~Context()
	{
	}

	Context& Context::GetInstance()
	{
		if (!instance_)
		{
			std::lock_guard<std::mutex> lock(singleton_mutex);
			if (!instance_)
			{
				instance_ = std::make_unique<Context>();
			}
		}
		return *instance_;
	}

	void Context::Init(void* wnd)
	{
		graphics_sys_ = std::make_unique<GraphicsSystem>(wnd);

		location_mgr_ = std::make_unique<LocationManager>();

		core_ = std::make_unique<Core>();

		RECT rc;
		::GetClientRect(static_cast<HWND>(wnd), &rc);
		core_->WindowHasBeenResized(static_cast<float>(rc.left), static_cast<float>(rc.top),
			static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));

		auto sol_system = std::make_unique<SolarSystem>();
		sol_system->Init();
		module_mgr_->RegisterModule(std::move(sol_system));

		// Core's observer depends on solar system module
		core_->Init();

		auto milky_way = std::make_unique<MilkyWay>();
		milky_way->Init();
		module_mgr_->RegisterModule(std::move(milky_way));

		initialized_ = true;
	}

	void Context::Deinit()
	{
		initialized_ = false;

		core_.reset();
		location_mgr_.reset();
		graphics_sys_.reset();
	}
	
	void Context::SetLocationManager(std::unique_ptr<LocationManager> loc_mgr)
	{
		location_mgr_ = std::move(loc_mgr);
	}

	LocationManager& Context::GetLocationManager()
	{
		return *location_mgr_;
	}

	ModuleManager& Context::GetModuleManager()
	{
		return *module_mgr_;
	}

	void Context::Update(double delta_time)
	{
		if (!initialized_)
		{
			return;
		}

		core_->Update(delta_time);

		module_mgr_->Update();

		for (auto* mod : module_mgr_->GetCallOrders(Module::Action::Update))
		{
			mod->Update(delta_time);
		}
	}

	void Context::Draw()
	{
		if (!initialized_)
		{
			return;
		}

		graphics_sys_->PreDraw();

		core_->PreDraw();

		for (auto* mod : module_mgr_->GetCallOrders(Module::Action::Draw))
		{
			mod->Draw(*core_);
		}

		core_->PostDraw();

		graphics_sys_->PostDraw();
	}
}
