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

#include <Sauron/Core.hpp>

#include <Sauron/Context.hpp>
#include <Sauron/Location.hpp>
#include <Sauron/LocationManager.hpp>
#include <Sauron/Observer.hpp>

namespace Sauron
{
	Core::Core()
	{
		auto& location_mgr = Context::GetInstance().GetLocationManager();
		auto location = location_mgr.GetLastLocation();
		location_mgr.LocationFromSystem();
		curr_observer_ = std::make_shared<Observer>(location);
	}

	void Core::Update(double delta_time)
	{
		(void)delta_time;
		// TODO
	}

	void Core::Render()
	{
		// TODO
	}

	void Core::SetCurrentObserver(std::shared_ptr<Observer> const & ob)
	{
		curr_observer_ = ob;
	}

	std::shared_ptr<Observer> const & Core::GetCurrentObserver() const
	{
		return curr_observer_;
	}

	void Core::MoveObserverTo(Location const & target)
	{
		this->SetCurrentObserver(std::make_shared<Observer>(target));
		// TODO: Send signals about target changed and location changed
	}
}
