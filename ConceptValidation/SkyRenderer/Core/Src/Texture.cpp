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

#include <Sauron/Texture.hpp>

#pragma warning(push)
#pragma warning(disable: 4244 4456)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

namespace Sauron
{
	Texture::Texture()
	{
		glGenTextures(1, &tex_);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &tex_);
	}

	void Texture::Load(std::string const & file_name)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		int w, h;
		stbi_set_flip_vertically_on_load(true);
		uint32_t* pixels = reinterpret_cast<uint32_t*>(stbi_load(file_name.c_str(), &w, &h, 0, 4));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		free(pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	void Texture::Bind(uint32_t stage)
	{
		glActiveTexture(GL_TEXTURE0 + stage);
		glBindTexture(GL_TEXTURE_2D, tex_);
	}
}
