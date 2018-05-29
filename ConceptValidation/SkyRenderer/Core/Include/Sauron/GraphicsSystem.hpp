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

#ifndef SAURON_SKY_RENDERER_CORE_GRAPHICS_SYSTEM_HPP
#define SAURON_SKY_RENDERER_CORE_GRAPHICS_SYSTEM_HPP

#pragma once

#define NOMINMAX
#include <EGL/egl.h>

#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>

namespace Sauron
{
	class GraphicsSystem
	{
		friend class Painter;

	public:
		explicit GraphicsSystem(void* wnd);
		~GraphicsSystem();

		void PreDraw();
		void PostDraw();

	private:
		void InitShaders();
		void DeinitShaders();

	private:
		EGLDisplay display_;
		EGLSurface surf_;
		EGLConfig cfg_;
		EGLContext context_;

		GLuint basic_shader_program_;
		struct BasicShaderVars
		{
			int projection_matrix;
			int color;
			int position;
		};
		BasicShaderVars basic_shader_vars_;

		GLuint color_shader_program_;
		BasicShaderVars color_shader_vars_;

		GLuint textures_shader_program_;
		struct TexturesShaderVars
		{
			int projection_matrix;
			int tex_coord;
			int position;
			int tex_color;
			int texture;
		};
		TexturesShaderVars textures_shader_vars_;

		GLuint textures_color_shader_program_;
		struct TexturesColorShaderVars
		{
			int projection_matrix;
			int tex_coord;
			int position;
			int color;
			int texture;
		};
		TexturesColorShaderVars textures_color_shader_vars_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_GRAPHICS_SYSTEM_HPP
