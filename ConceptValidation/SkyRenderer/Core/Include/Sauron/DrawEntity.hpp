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

#ifndef SAURON_SKY_RENDERER_CORE_DRAW_ENTITY_HPP
#define SAURON_SKY_RENDERER_CORE_DRAW_ENTITY_HPP

#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Sauron
{
	class DrawEntity
	{
	public:
		enum class PrimitiveType
		{
			Points = 0x0000, // GL_POINTS
			Lines = 0x0001, // GL_LINES
			LineLoop = 0x0002, // GL_LINE_LOOP
			LineStrip = 0x0003, // GL_LINE_STRIP
			Triangles = 0x0004, // GL_TRIANGLES
			TriangleStrip = 0x0005, // GL_TRIANGLE_STRIP
			TriangleFan = 0x0006  // GL_TRIANGLE_FAN
		};

	public:
		DrawEntity()
			: DrawEntity(PrimitiveType::Triangles)
		{
		}
		DrawEntity(PrimitiveType type)
			: primitive_type_(type)
		{
		}
		DrawEntity(PrimitiveType type, std::vector<glm::vec3> const & positions)
			: DrawEntity(type, positions, std::vector<glm::vec2>())
		{
		}
		DrawEntity(PrimitiveType type, std::vector<glm::vec3> const & positions, std::vector<glm::vec2> const & tex_coords)
			: DrawEntity(type, positions, tex_coords, std::vector<uint16_t>())
		{
		}
		DrawEntity(PrimitiveType type, std::vector<glm::vec3> const & positions, std::vector<glm::vec2> const & tex_coords,
			std::vector<uint16_t> const & indices)
			: primitive_type_(type), positions_(positions), tex_coords_(tex_coords), indices_(indices)
		{
		}

		PrimitiveType GetPrimitiveType() const
		{
			return primitive_type_;
		}

		std::vector<glm::vec3>& GetPositions()
		{
			return positions_;
		}
		std::vector<glm::vec3> const & GetPositions() const
		{
			return positions_;
		}

		std::vector<glm::vec2>& GetTexCoords()
		{
			return tex_coords_;
		}
		std::vector<glm::vec2> const & GetTexCoords() const
		{
			return tex_coords_;
		}

		std::vector<glm::vec3>& GetColors()
		{
			return colors_;
		}
		std::vector<glm::vec3> const & GetColors() const
		{
			return colors_;
		}
		std::vector<uint16_t>& GetIndices()
		{
			return indices_;
		}
		std::vector<uint16_t> const & GetIndices() const
		{
			return indices_;
		}

		bool IsIndexed() const
		{
			return !indices_.empty();
		}

		bool IsTextured() const
		{
			return !tex_coords_.empty();
		}

		bool IsColored() const
		{
			return !colors_.empty();
		}

	private:
		PrimitiveType primitive_type_;

		std::vector<glm::vec3> positions_;
		std::vector<glm::vec2> tex_coords_;
		std::vector<glm::vec3> colors_;
		std::vector<uint16_t> indices_;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_DRAW_ENTITY_HPP
