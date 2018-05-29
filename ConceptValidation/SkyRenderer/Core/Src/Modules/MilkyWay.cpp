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

#include "MilkyWay.hpp"

#include <algorithm>

#include <boost/assert.hpp>

#include <Sauron/Context.hpp>
#include <Sauron/Core.hpp>
#include <Sauron/Painter.hpp>
#include <Sauron/Projector.hpp>
#include <Sauron/Texture.hpp>
#include <Sauron/ModuleManager.hpp>
#include <Sauron/Util.hpp>

namespace Sauron
{
	//! Generate a DrawEntity for a sphere.
	//! @param radius
	//! @param oneMinusOblateness
	//! @param slices: number of vertical segments ("meridian zones")
	//! @param stacks: number of horizontal segments ("latitude zones")
	//! @param orient_inside: 1 to have normals point inside, e.g. for Milky Way, Zodiacal Light, etc.
	//! @param flip_texture: if texture should be mapped to inside of sphere, e.g. Milky Way.
	//! @param top_angle: An opening angle [radians] at top of the sphere. Useful if there is an empty
	//!        region around the top pole, like North Galactic Pole.
	//! @param bottom_angle: An opening angle [radians] at bottom of the sphere. Useful if there is an empty
	//!        region around the bottom pole, like South Galactic Pole.
	DrawEntity ComputeSphereNoLight(float radius, float one_minus_oblateness, int slices, int stacks,
		int orient_inside, bool flip_texture, float top_angle = 0, float bottom_angle = M_PI)
	{
		BOOST_ASSERT(top_angle < bottom_angle); // don't forget: These are opening angles counted from top.

		DrawEntity ret(DrawEntity::PrimitiveType::Triangles);
		float x, y, z;
		float s = 0.f, t = 0.f;
		int i, j;
		float nsign;
		if (orient_inside)
		{
			nsign = -1;
			t = 0; // from inside texture is reversed
		}
		else
		{
			nsign = 1;
			t = 1;
		}

		float const * cos_sin_rho = nullptr;
		if ((bottom_angle > M_PI - 0.0001f) && (top_angle < 0.0001f))
		{
			cos_sin_rho = ComputeCosSinRho(stacks);
		}
		else
		{
			// delta_rho:  originally just 180degrees/stacks, now the range clamped.
			float const delta_rho = (bottom_angle - top_angle) / stacks;
			cos_sin_rho = ComputeCosSinRhoZone(delta_rho, stacks, static_cast<float>(M_PI - bottom_angle));
		}
		// Allow parameters so that pole regions may remain free.
		float const * cos_sin_rho_p;

		float const * cos_sin_theta = ComputeCosSinTheta(slices);
		float const * cos_sin_theta_p;

		// texturing: s goes from 0.0/0.25/0.5/0.75/1.0 at +y/+x/-y/-x/+y axis
		// t goes from -1.0/+1.0 at z = -radius/+radius (linear along longitudes)
		// cannot use triangle fan on texturing (s coord. at top/bottom tip varies)
		// If the texture is flipped, we iterate the coordinates backward.
		float const ds = (flip_texture ? -1.0f : 1.0f) / slices;
		float const dt = nsign / stacks; // from inside texture is reversed

		// draw intermediate as quad strips
		for (i = 0, cos_sin_rho_p = cos_sin_rho; i < stacks; ++ i, cos_sin_rho_p += 2)
		{
			s = flip_texture ? 1.0f : 0.0f;
			for (j = 0, cos_sin_theta_p = cos_sin_theta; j <= slices; ++ j, cos_sin_theta_p += 2)
			{
				x = -cos_sin_theta_p[1] * cos_sin_rho_p[1];
				y = cos_sin_theta_p[0] * cos_sin_rho_p[1];
				z = nsign * cos_sin_rho_p[0];
				ret.GetTexCoords().push_back(glm::vec2(s, t));
				ret.GetPositions().push_back(glm::vec3(x * radius, y * radius, z * one_minus_oblateness * radius));
				x = -cos_sin_theta_p[1] * cos_sin_rho_p[3];
				y = cos_sin_theta_p[0] * cos_sin_rho_p[3];
				z = nsign * cos_sin_rho_p[2];
				ret.GetTexCoords().push_back(glm::vec2(s, t - dt));
				ret.GetPositions().push_back(glm::vec3(x * radius, y * radius, z * one_minus_oblateness * radius));
				s += ds;
			}
			uint32_t const offset = i * (slices + 1) * 2;
			for (j = 2; j < slices * 2 + 2; j += 2)
			{
				ret.GetIndices().push_back(static_cast<uint16_t>(offset + j - 2));
				ret.GetIndices().push_back(static_cast<uint16_t>(offset + j - 1));
				ret.GetIndices().push_back(static_cast<uint16_t>(offset + j));

				ret.GetIndices().push_back(static_cast<uint16_t>(offset + j));
				ret.GetIndices().push_back(static_cast<uint16_t>(offset + j - 1));
				ret.GetIndices().push_back(static_cast<uint16_t>(offset + j + 1));
			}
			t -= dt;
		}

		return ret;
	}

	// Class which manages the displaying of the Milky Way
	MilkyWay::MilkyWay()
		: Module("MilkyWay")
	{
	}

	MilkyWay::~MilkyWay()
	{
		this->Deinit();
	}

	void MilkyWay::Init()
	{
		tex_ = std::make_shared<Texture>();
		// The texture is from Stellarium.
		tex_->Load("Data/Textures/Milkyway.png");

		draw_entity_ = std::make_unique<DrawEntity>(ComputeSphereNoLight(1, 1, 45, 15, 1, true));
		draw_entity_->GetColors().assign(draw_entity_->GetPositions().size(), glm::vec3(1.0f, 0.3f, 0.9f));
	}

	void MilkyWay::Deinit()
	{
		draw_entity_.reset();
		tex_.reset();
	}

	float MilkyWay::GetCallOrder(Action action) const
	{
		if (action == Module::Action::Draw)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	void MilkyWay::Draw(Core& core)
	{
		if (!this->GetVisible())
		{
			return;
		}

		std::shared_ptr<Projector::ModelViewTransform> transfom = core.GetJ2000ModelViewTransform();

		auto const & projector = core.GetProjection(transfom);
		auto* drawer = core.GetSkyDrawer();

		//BOOST_ASSERT(tex_);	// A texture must be loaded before calling this

		// This RGB color corresponds to the night blue scotopic color = 0.25, 0.25 in xyY mode.
		// since milky way is always seen white RGB value in the texture (1.0, 1.0, 1.0)
		glm::vec3 c = color_;

		// We must also adjust milky way to light pollution.
		// Is there any way to calibrate this?
		int bortle = drawer->GetBortleScaleIndex();

		float lum = drawer->SurfaceBrightnessToLuminance(12 + 0.15f * bortle); // was 11.5; Source? How to calibrate the new texture?

		// Bound a maximum luminance. Is there any reference/reason, or just trial and error?
		lum = std::min(0.38f, lum * 2);

		// intensity of 1.0 is "proper", but allow boost for dim screens
		c *= lum * intensity_;

		// TODO: Find an even better balance with sky brightness, MW should be hard to see during Full Moon and at least somewhat reduced in smaller phases.
		// adapt brightness by atmospheric brightness. This block developed for ZodiacalLight, hopefully similarly applicable...
		float const atm_lum = 0.000616604288f;// TODO: GET_MODULE(LandscapeManager)->GetAtmosphereAverageLuminance();
		// 10cd/m^2 at sunset, 3.3 at civil twilight (sun at -6deg). 0.0145 sun at -12, 0.0004 sun at -18,  0.01 at Full Moon!?
		float atm_factor = std::max(0.35f, 50.0f * (0.02f - atm_lum)); // keep visible in twilight, but this is enough for some effect with the moon.
		c *= atm_factor * atm_factor;

		if (c[0] < 0)
		{
			c[0] = 0;
		}
		if (c[1] < 0)
		{
			c[1] = 0;
		}
		if (c[2] < 0)
		{
			c[2] = 0;
		}

		bool const with_extinction = (drawer->ShowAtmosphere() && drawer->GetExtinction().GetExtinctionCoefficient() >= 0.01f);
		if (with_extinction)
		{
			// We must process the vertices to find geometric altitudes in order to compute vertex colors.
			// Note that there is a visible boost of extinction for higher Bortle indices. I must reflect that as well.
			auto const & extinction = drawer->GetExtinction();

			for (int i = 0; i < draw_entity_->GetPositions().size(); ++ i)
			{
				glm::dvec3 vert_alt_az = core.J2000ToAltAz(draw_entity_->GetPositions()[i], Core::RefractionMode::On);
				BOOST_ASSERT(std::abs(glm::dot(vert_alt_az, vert_alt_az) - 1.0) < 0.001);

				float one_mag = 0;
				extinction.Forward(vert_alt_az, &one_mag);
				// drop of one magnitude: should be factor 2.5 or 40%. We take 30%, it looks more realistic.
				float extinction_factor = std::pow(0.3f, one_mag) * (1.1f - bortle * 0.1f);
				draw_entity_->GetColors()[i] = c * extinction_factor;
			}
		}
		else
		{
			draw_entity_->GetColors().assign(draw_entity_->GetPositions().size(), c);
		}

		Painter painter(projector);
		painter.SetCullFace(true);
		painter.SetBlending(false);
		tex_->Bind(0);
		painter.Draw(*draw_entity_);
	}
}
