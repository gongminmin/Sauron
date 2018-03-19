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

#ifndef SAURON_SKY_RENDERER_CORE_PROJECTOR_HPP
#define SAURON_SKY_RENDERER_CORE_PROJECTOR_HPP

#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Sauron
{
	//! @class Projector
	//! Provide the main interface to all operations of projecting coordinates from sky to screen.
	class Projector
	{
	public:
		//! @struct ProjectorParams
		//! Contains all the param needed to initialize a Projector
		struct ProjectorParams
		{
			glm::tvec4<int> viewport_xy_wh = glm::tvec4<int>(0, 0, 256, 256);	//! pos x, pos y, width, height
			float fov = 60;														//! FOV in degrees
			float z_near = 0;													//! Near clipping plane
			float z_far = 0;													//! Far clipping planes
			glm::vec2 viewport_center = glm::vec2(128, 128);					//! Viewport center in screen pixel
			float viewport_fov_diameter = 0;									//! diameter of the FOV disk in pixel
		};

		//! @class ModelViewTranform
		//! Allows to define non linear operations in addition to the standard linear (Matrix 4d) ModelView transformation.
		class ModelViewTranform
		{
		public:
			explicit ModelViewTranform(glm::mat4 const & m);

			void Forward(glm::vec3& v) const;
			void Backward(glm::vec3& v) const;

			void Combine(glm::mat4 const & m);

			glm::mat4 const & GetTransformMatrix() const;

		private:
			glm::mat4 transform_matrix_;
		};

		explicit Projector(std::shared_ptr<ModelViewTranform> const & model_view, ProjectorParams const & params);
		virtual ~Projector() = default;

		//! Get the maximum FOV apperture in degree
		virtual float GetMaxFov() const = 0;
		//! Apply the transformation in the forward direction in place.
		//! After transformation v[2] will always contain the length of the original v: sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])
		//! regardless of the projection type. This makes it possible to implement depth buffer testing in a way independent of the
		//! projection type. I would like to return the squared length instead of the length because of performance reasons.
		//! But then far away objects are not textured any more, perhaps because of a depth buffer overflow although
		//! the depth test is disabled?
		virtual bool Forward(glm::vec3& v) const = 0;
		//! Apply the transformation in the backward projection in place.
		virtual bool Backward(glm::vec3& v) const = 0;
		//! Return the small zoom increment to use at the given FOV for nice movements
		virtual float DeltaZoom(float fov) const = 0;

		//! Convert a Field of View radius value in radians in ViewScalingFactor (used internally)
		virtual float FovToViewScalingFactor(float fov) const = 0;
		//! Convert a ViewScalingFactor value (used internally) in Field of View radius in radians
		virtual float ViewScalingFactorToFov(float vsf) const = 0;

		//! Get the lower left corner of the viewport and the width, height.
		glm::tvec4<int> const & GetViewport() const
		{
			return params_.viewport_xy_wh;
		}

		glm::vec2 GetViewportCenter() const;

		//! Get the horizontal viewport offset in pixels.
		int GetViewportPosX() const
		{
			return params_.viewport_xy_wh.x;
		}

		//! Get the vertical viewport offset in pixels.
		int GetViewportPosY() const
		{
			return params_.viewport_xy_wh.y;
		}

		//! Get the viewport width in pixels.
		int GetViewportWidth() const
		{
			return params_.viewport_xy_wh.z;
		}
		//! Get the viewport height in pixels.
		int GetViewportHeight() const
		{
			return params_.viewport_xy_wh.w;
		}

		//! Get the current FOV diameter in degrees
		float GetFov() const
		{
			params_.fov;
		}

		//! Project the vector v from the current frame into the viewport.
		//! @param v the vector in the current frame.
		//! @param win the projected vector in the viewport 2D frame.
		//! @return true if the projected coordinate is valid.
		virtual bool Project(glm::vec3 const & v, glm::vec3& win) const;

		//! Project the vector v from the current frame into the viewport.
		//! @param vd the vector in the current frame.
		//! @return true if the projected coordinate is valid.
		bool ProjectInPlace(glm::vec3& vd) const;

		//! Project the vector v from the viewport frame into the current frame.
		//! @param win the vector in the viewport 2D frame. win[0] and win[1] are in screen pixels, win[2] is unused.
		//! @param v the unprojected direction vector in the current frame.
		//! @return true if the projected coordinate is valid.
		bool Unproject(glm::vec3 const & win, glm::vec3& v) const;
		bool Unproject(float x, float y, glm::vec3& v) const;

	protected:
		std::shared_ptr<ModelViewTranform> model_view_transform_;

		ProjectorParams params_;
		float pixel_per_rad_ = 0;				// pixel per rad at the center of the viewport disk
		float one_over_z_near_minus_far_ = 0;
	};

	class ProjectorPerspective : public Projector
	{
	public:
		ProjectorPerspective(std::shared_ptr<ModelViewTranform> const & model_view, ProjectorParams const & params);

		float GetMaxFov() const override
		{
			return 120;
		}

		bool Forward(glm::vec3& v) const override;
		bool Backward(glm::vec3& v) const override;
		float DeltaZoom(float fov) const override;

		float FovToViewScalingFactor(float fov) const override;
		float ViewScalingFactorToFov(float vsf) const override;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_PROJECTOR_HPP
