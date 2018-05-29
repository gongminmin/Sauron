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

#include <Sauron/SkyDrawer.hpp>

#include <iostream>

#include <Sauron/Util.hpp>

namespace Sauron
{
	SkyDrawer::SkyDrawer(Core* core)
		: core_(core)
	{
	}

	SkyDrawer::~SkyDrawer()
	{
	}

	void SkyDrawer::Init()
	{
		// TODO

		show_atmosphere_ = true;
	}

	void SkyDrawer::Update(double delta_time)
	{
		SAURON_UNUSED(delta_time);
	}

	// Set the parameters so that the stars disappear at about the limit given by the bortle scale
	// See http://en.wikipedia.org/wiki/Bortle_Dark-Sky_Scale
	void SkyDrawer::SetBortleScaleIndex(int b_index)
	{
		if (bortle_scale_index_ != b_index)
		{
			// Associate the Bortle index (1 to 9) to inscale value
			if (b_index < 1)
			{
				std::cerr << "WARNING: Bortle scale index range is [1;9], given" << b_index;
				b_index = 1;
			}
			if (b_index > 9)
			{
				std::cerr << "WARNING: Bortle scale index range is [1;9], given" << b_index;
				b_index = 9;
			}

			bortle_scale_index_ = b_index;
		}
	}

	float SkyDrawer::GetNELMFromBortleScale() const
	{
		float nelm = 0.f;
		switch (bortle_scale_index_)
		{
		case 1:
			nelm = 7.8f; // Class 1 = NELM 7.6每8.0; average NELM is 7.8
			break;
		case 2:
			nelm = 7.3f; // Class 2 = NELM 7.1每7.5; average NELM is 7.3
			break;
		case 3:
			nelm = 6.8f; // Class 3 = NELM 6.6每7.0; average NELM is 6.8
			break;
		case 4:
			nelm = 6.3f; // Class 4 = NELM 6.1每6.5; average NELM is 6.3
			break;
		case 5:
			nelm = 5.8f; // Class 5 = NELM 5.6每6.0; average NELM is 5.8
			break;
		case 6:
			nelm = 5.3f; // Class 6 = NELM 5.1-5.5; average NELM is 5.3
			break;
		case 7:
			nelm = 4.8f; // Class 7 = NELM 4.6每5.0; average NELM is 4.8
			break;
		case 8:
			nelm = 4.3f; // Class 8 = NELM 4.1每4.5; average NELM is 4.3
			break;
		case 9:
			nelm = 4.0f; // Class 8 = NELM 4.0
			break;

		default:
			break;
		}
		return nelm;
	}
}
