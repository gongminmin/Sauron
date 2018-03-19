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

#include <Sauron/Projector.hpp>

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace Sauron
{
	Projector::ModelViewTranform::ModelViewTranform(glm::mat4 const & m)
		: transform_matrix_(m)
	{
	}

	void Projector::ModelViewTranform::Forward(glm::vec3& v) const
	{
		glm::vec4 v4 = transform_matrix_ * glm::vec4(v.x, v.y, v.z, 1.0f);
		v4 /= v4.w;
		v = glm::vec3(v4.x, v4.y, v4.z);
	}

	void Projector::ModelViewTranform::Backward(glm::vec3& v) const
	{
		// We need no matrix inversion because we always work with orthogonal matrices (where the transposed is the inverse).
		glm::vec4 v4 = glm::transpose(transform_matrix_) * glm::vec4(v.x, v.y, v.z, 1.0f);
		v4 /= v4.w;
		v = glm::vec3(v4.x, v4.y, v4.z);
	}

	void Projector::ModelViewTranform::Combine(glm::mat4 const & m)
	{
		transform_matrix_ *= m;
	}

	glm::mat4 const & Projector::ModelViewTranform::GetTransformMatrix() const
	{
		return transform_matrix_;
	}


	Projector::Projector(std::shared_ptr<ModelViewTranform> const & model_view, ProjectorParams const & params)
		: model_view_transform_(model_view), params_(params),
			pixel_per_rad_(0.5f * params_.viewport_fov_diameter
				/ this->FovToViewScalingFactor(params_.fov * static_cast<float>(M_PI / 360))),
			one_over_z_near_minus_far_(1 / (params.z_near - params.z_far))
	{
		// TODO: Handle DPI?
	}
	
	glm::vec2 Projector::GetViewportCenter() const
	{
		return glm::vec2(params_.viewport_center.x - params_.viewport_xy_wh.x, params_.viewport_center.y - params_.viewport_xy_wh.y);
	}

	bool Projector::Project(glm::vec3 const & v, glm::vec3& win) const
	{
		win = v;
		return this->ProjectInPlace(win);
	}

	bool Projector::ProjectInPlace(glm::vec3& v) const
	{
		model_view_transform_->Forward(v);
		bool const rval = this->Forward(v);
		v.x = params_.viewport_center.x + pixel_per_rad_ * v.x;
		v.y = params_.viewport_center.y + pixel_per_rad_ * v.y;
		v.z = (v.z - params_.z_near) * one_over_z_near_minus_far_;
		return rval;
	}

	bool Projector::Unproject(glm::vec3 const & win, glm::vec3& v) const
	{
		return this->Unproject(win[0], win[1], v);
	}

	bool Projector::Unproject(float x, float y, glm::vec3& v) const
	{
		v[0] = (x - params_.viewport_center.x) / pixel_per_rad_;
		v[1] = (y - params_.viewport_center.y) / pixel_per_rad_;
		v[2] = 0;
		const bool rval = this->Backward(v);
		model_view_transform_->Backward(v);
		return rval;
	}


	ProjectorPerspective::ProjectorPerspective(std::shared_ptr<ModelViewTranform> const & model_view, ProjectorParams const & params)
		: Projector(model_view, params)
	{
	}

	bool ProjectorPerspective::Forward(glm::vec3& v) const
	{
		float const r = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		if (v[2] < 0)
		{
			v[0] *= (-1 / v[2]);
			v[1] /= (-v[2]);
			v[2] = r;
			return true;
		}
		if (v[2] > 0)
		{
			v[0] *= 1 / v[2];
			v[1] /= v[2];
			v[2] = -std::numeric_limits<float>::max();
			return false;
		}
		v[0] = std::numeric_limits<float>::max();
		v[1] = std::numeric_limits<float>::max();
		v[2] = -std::numeric_limits<float>::max();
		return false;
	}

	bool ProjectorPerspective::Backward(glm::vec3& v) const
	{
		v[2] = std::sqrt(1 / (1 + v[0] * v[0] + v[1] * v[1]));
		v[0] *= v[2];
		v[1] *= v[2];
		v[2] = -v[2];
		return true;
	}

	float ProjectorPerspective::DeltaZoom(float fov) const
	{
		float const vsf = this->FovToViewScalingFactor(fov);
		return vsf / (1 + vsf * vsf);
	}

	float ProjectorPerspective::FovToViewScalingFactor(float fov) const
	{
		return std::tan(fov);
	}

	float ProjectorPerspective::ViewScalingFactorToFov(float vsf) const
	{
		return std::atan(vsf);
	}
}
