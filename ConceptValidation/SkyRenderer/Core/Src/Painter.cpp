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

#include <Sauron/Painter.hpp>

#include <algorithm>
#include <iostream>

#include <boost/assert.hpp>

#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>

#include <Sauron/Context.hpp>
#include <Sauron/DrawEntity.hpp>
#include <Sauron/GraphicsSystem.hpp>

namespace Sauron
{
	Painter::Painter(std::shared_ptr<Projector> const & projector)
		: projector_(projector)
	{
		BOOST_ASSERT(projector);
	}

	Painter::~Painter()
	{
	}

	void Painter::SetBlending(bool enable_blending, GLenum blend_src, GLenum blend_dst)
	{
		if (enable_blending)
		{
			glEnable(GL_BLEND);
			glBlendFunc(blend_src, blend_dst);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void Painter::SetCullFace(bool enable)
	{
		if (enable)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	void Painter::Draw(DrawEntity& entity)
	{
		auto& graphics_sys = Context::GetInstance().GetGraphicsSystem();

		std::vector<glm::vec3> projected_positions;
		if (entity.IsIndexed())
		{
			// we need to find the max value of the indices
			uint16_t max = 0;
			for (size_t i = 0; i < entity.GetIndices().size(); ++ i)
			{
				max = std::max(max, entity.GetIndices()[i]);
			}
			projected_positions.resize(max + 1);
			for (size_t i = 0; i < max + 1; ++ i)
			{
				glm::dvec3 tmp;
				projector_->Project(entity.GetPositions()[i], tmp);
				projected_positions[i] = glm::vec3(tmp);
			}
		}
		else
		{
			projected_positions.resize(entity.GetPositions().size());
			for (size_t i = 0; i < projected_positions.size(); ++ i)
			{
				glm::dvec3 tmp;
				projector_->Project(entity.GetPositions()[i], tmp);
				projected_positions[i] = glm::vec3(tmp);
			}
		}

		auto const & proj_mat = projector_->GetProjectionMatrix();

		if (!entity.IsTextured() && !entity.IsColored())
		{
			glUseProgram(graphics_sys.basic_shader_program_);

			glVertexAttribPointer(graphics_sys.basic_shader_vars_.position, 3, GL_FLOAT, false, sizeof(glm::vec3),
				projected_positions.data());
			glEnableVertexAttribArray(graphics_sys.basic_shader_vars_.position);

			glUniformMatrix4fv(graphics_sys.basic_shader_vars_.projection_matrix, 1, false, &proj_mat[0][0]);
			glUniform4fv(graphics_sys.basic_shader_vars_.color, 1, &curr_color_.x);
		}
		else if (!entity.IsTextured() && entity.IsColored())
		{
			glUseProgram(graphics_sys.color_shader_program_);

			glVertexAttribPointer(graphics_sys.color_shader_vars_.position, 3, GL_FLOAT, false, sizeof(glm::vec3),
				projected_positions.data());
			glEnableVertexAttribArray(graphics_sys.color_shader_vars_.position);
			glVertexAttribPointer(graphics_sys.color_shader_vars_.color, 3, GL_FLOAT, false, sizeof(glm::vec3),
				entity.GetColors().data());
			glEnableVertexAttribArray(graphics_sys.color_shader_vars_.color);

			glUniformMatrix4fv(graphics_sys.color_shader_vars_.projection_matrix, 1, false, &proj_mat[0][0]);
		}
		else if (entity.IsTextured() && !entity.IsColored())
		{
			glUseProgram(graphics_sys.textures_shader_program_);

			glVertexAttribPointer(graphics_sys.textures_shader_vars_.position, 3, GL_FLOAT, false, sizeof(glm::vec3),
				projected_positions.data());
			glEnableVertexAttribArray(graphics_sys.textures_shader_vars_.position);
			glVertexAttribPointer(graphics_sys.textures_shader_vars_.tex_coord, 2, GL_FLOAT, false, sizeof(glm::vec2),
				entity.GetTexCoords().data());
			glEnableVertexAttribArray(graphics_sys.textures_shader_vars_.tex_coord);

			glUniformMatrix4fv(graphics_sys.textures_shader_vars_.projection_matrix, 1, false, &proj_mat[0][0]);
			glUniform4fv(graphics_sys.textures_shader_vars_.tex_color, 1, &curr_color_.x);

			glUniform1i(graphics_sys.textures_shader_vars_.texture, 0);
		}
		else if (entity.IsTextured() && entity.IsColored())
		{
			glUseProgram(graphics_sys.textures_color_shader_program_);

			glVertexAttribPointer(graphics_sys.textures_color_shader_vars_.position, 3, GL_FLOAT, false, sizeof(glm::vec3),
				projected_positions.data());
			glEnableVertexAttribArray(graphics_sys.textures_color_shader_vars_.position);
			glVertexAttribPointer(graphics_sys.textures_color_shader_vars_.tex_coord, 2, GL_FLOAT, false, sizeof(glm::vec2),
				entity.GetTexCoords().data());
			glEnableVertexAttribArray(graphics_sys.textures_color_shader_vars_.tex_coord);
			glVertexAttribPointer(graphics_sys.textures_color_shader_vars_.color, 3, GL_FLOAT, false, sizeof(glm::vec3),
				entity.GetColors().data());
			glEnableVertexAttribArray(graphics_sys.textures_color_shader_vars_.color);

			glUniformMatrix4fv(graphics_sys.textures_color_shader_vars_.projection_matrix, 1, false, &proj_mat[0][0]);

			glUniform1i(graphics_sys.textures_color_shader_vars_.texture, 0);
		}

		if (entity.IsIndexed())
		{
			glDrawElements(static_cast<GLenum>(entity.GetPrimitiveType()),
				static_cast<GLsizei>(entity.GetIndices().size()), GL_UNSIGNED_SHORT, entity.GetIndices().data());
		}
		else
		{
			glDrawArrays(static_cast<GLenum>(entity.GetPrimitiveType()), 0, static_cast<GLsizei>(projected_positions.size()));
		}
	}
}
