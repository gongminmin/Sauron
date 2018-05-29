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

#ifndef SAURON_SKY_RENDERER_CORE_PAINTER_HPP
#define SAURON_SKY_RENDERER_CORE_PAINTER_HPP

#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>

#include <Sauron/Projector.hpp>

namespace Sauron
{
	class DrawEntity;

	class Painter
	{
	public:
		explicit Painter(std::shared_ptr<Projector> const & projector);
		~Painter();

		//! Enable blending. By default, blending is disabled.
		//! The additional parameters specify the blending mode, the default parameters are suitable for
		//! "normal" blending operations that you want in most cases. Blending will be automatically disabled when
		//! the Painter is destroyed.
		void SetBlending(bool enable_blending, GLenum blend_src = GL_SRC_ALPHA, GLenum blend_dst = GL_ONE_MINUS_SRC_ALPHA);

		//! Set the GL_CULL_FACE state, by default face culling is disabled
		void SetCullFace(bool enable);

		//! Set the color to use for subsequent drawing.
		void SetColor(float r, float g, float b, float a)
		{
			curr_color_ = glm::vec4(r, g, b, a);
		}

		//! Get the color currently used for drawing.
		glm::vec4 const & GetColor() const
		{
			return curr_color_;
		}

		//! Draws the primitives defined in the DrawEntity.
		void Draw(DrawEntity& entity);

	private:
		std::shared_ptr<Projector> const & projector_;

		glm::vec4 curr_color_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_PAINTER_HPP
