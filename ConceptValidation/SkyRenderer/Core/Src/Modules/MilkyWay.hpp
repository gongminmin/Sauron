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

#ifndef SAURON_SKY_RENDERER_CORE_MODULES_MILKYWAY_HPP
#define SAURON_SKY_RENDERER_CORE_MODULES_MILKYWAY_HPP

#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>

#include <glm/vec3.hpp>

#include <Sauron/DrawEntity.hpp>
#include <Sauron/Module.hpp>

namespace Sauron
{
	class Texture;

	//! @class MilkyWay 
	//! Manages the displaying of the Milky Way.
	class MilkyWay : public Module
	{
	public:
		MilkyWay();
		~MilkyWay() override;
	
		//! Initialize the class.  Here we load the texture for the Milky Way and 
		//! get the display settings from application settings, namely the flag which
		//! determines if the Milky Way is displayed or not, and the intensity setting.
		void Init() override;

		void Deinit() override;

		//! Draw the Milky Way.
		void Draw(Core& core) override;

		//! Update and time-dependent state.  Updates the fade level while the 
		//! Milky way rendering is being changed from on to off or off to on.
		void Update(double delta_time) override
		{
			SAURON_UNUSED(delta_time);
		}
	
		//! Action::Draw returns 1 (because this is background, very early drawing).
		//! Other actions return 0 for no action.
		float GetCallOrder(Action action) const override;
	
	public:
		//! Get Milky Way intensity.
		float GetIntensity() const
		{
			return intensity_;
		}

		//! Set Milky Way intensity. Default value: 1.
		void SetIntensity(float intensity)
		{
			intensity_ = intensity;
		}
	
		//! Get the color used for rendering the Milky Way. It is modulated by intensity, light pollution and atmospheric extinction.
		glm::vec3 GetColor() const
		{
			return color_;
		}

		//! Sets the color to use for rendering the Milky Way
		//! @param c The color to use for rendering the Milky Way. Default (1.0, 1.0, 1.0)
		void SetColor(glm::vec3 const & c)
		{
			color_ = c;
		}

		//! Gets whether the Milky Way is displayed
		bool GetVisible() const
		{
			return visible_;
		}

		//! Sets whether to show the Milky Way
		void SetVisible(bool vis)
		{
			visible_ = vis;
		}

	private:
		bool visible_ = true;

		std::shared_ptr<Texture> tex_;
		glm::vec3 color_ = glm::vec3(1, 1, 1); // global color
		float intensity_ = 1;

		std::unique_ptr<DrawEntity> draw_entity_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_MODULES_MILKYWAY_HPP
