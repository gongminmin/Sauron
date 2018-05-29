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

#include <Sauron/Util.hpp>

namespace Sauron
{
	Projector::Mat4Transform::Mat4Transform(glm::dmat4 const & m)
		: transform_matrix_(m)
	{
	}

	void Projector::Mat4Transform::Forward(glm::dvec3& v) const
	{
		v = transform_matrix_ * glm::dvec4(v, 1);
	}

	void Projector::Mat4Transform::Backward(glm::dvec3& v) const
	{
		// We need no matrix inversion because we always work with orthogonal matrices (where the transposed is the inverse).
		double const x = v[0] - transform_matrix_[0][3];
		double const y = v[1] - transform_matrix_[1][3];
		double const z = v[2] - transform_matrix_[2][3];
		v[0] = transform_matrix_[0][0] * x + transform_matrix_[1][0] * y + transform_matrix_[2][0] * z;
		v[1] = transform_matrix_[0][1] * x + transform_matrix_[1][1] * y + transform_matrix_[2][1] * z;
		v[2] = transform_matrix_[0][2] * x + transform_matrix_[1][2] * y + transform_matrix_[2][2] * z;
	}

	void Projector::Mat4Transform::Combine(glm::dmat4 const & m)
	{
		transform_matrix_ *= m;
	}

	std::shared_ptr<Projector::ModelViewTransform> Projector::Mat4Transform::Clone() const
	{
		return std::make_shared<Mat4Transform>(transform_matrix_);
	}

	glm::dmat4 Projector::Mat4Transform::GetTransformMatrix() const
	{
		return transform_matrix_;
	}


	Projector::Projector(std::shared_ptr<ModelViewTransform> const & model_view)
		: model_view_transform_(model_view)
	{
	}

	void Projector::Init(ProjectorParams const & params)
	{
		params_ = params;
		
		pixel_per_rad_ = 0.5f * params_.viewport_fov_diameter / this->FovToViewScalingFactor(Deg2Rad(params_.fov / 2));
		one_over_z_near_minus_far_ = 1 / (params.z_near - params.z_far);

		// TODO: Handle DPI?
	}
	
	glm::vec2 Projector::GetViewportCenter() const
	{
		return glm::vec2(params_.viewport_center.x - params_.viewport_xy_wh.x, params_.viewport_center.y - params_.viewport_xy_wh.y);
	}

	bool Projector::Project(glm::dvec3 const & v, glm::dvec3& win) const
	{
		win = v;
		return this->ProjectInPlace(win);
	}

	bool Projector::ProjectInPlace(glm::dvec3& v) const
	{
		model_view_transform_->Forward(v);
		bool const rval = this->Forward(v);
		v.x = params_.viewport_center.x + pixel_per_rad_ * v.x;
		v.y = params_.viewport_center.y + pixel_per_rad_ * v.y;
		v.z = (v.z - params_.z_near) * one_over_z_near_minus_far_;
		return rval;
	}

	bool Projector::Unproject(glm::dvec3 const & win, glm::dvec3& v) const
	{
		return this->Unproject(win.x, win.y, v);
	}

	bool Projector::Unproject(double x, double y, glm::dvec3& v) const
	{
		v[0] = (x - params_.viewport_center.x) / pixel_per_rad_;
		v[1] = (y - params_.viewport_center.y) / pixel_per_rad_;
		v[2] = 0;
		bool const rval = this->Backward(v);
		model_view_transform_->Backward(v);
		return rval;
	}

	glm::mat4 Projector::GetProjectionMatrix() const
	{
		return glm::mat4(
			2.0f / params_.viewport_xy_wh.z, 0, 0, 0,
			0, 2.0f / params_.viewport_xy_wh.w, 0, 0,
			0, 0, -1, 0,
			-(2.0f * params_.viewport_xy_wh.x + params_.viewport_xy_wh.z) / params_.viewport_xy_wh.z,
			-(2.0f * params_.viewport_xy_wh.y + params_.viewport_xy_wh.w) / params_.viewport_xy_wh.w, 0, 1);
	}


	ProjectorPerspective::ProjectorPerspective(std::shared_ptr<ModelViewTransform> const & model_view)
		: Projector(model_view)
	{
	}

	bool ProjectorPerspective::Forward(glm::dvec3& v) const
	{
		double const r = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
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
			v[2] = -std::numeric_limits<double>::max();
			return false;
		}
		v[0] = std::numeric_limits<double>::max();
		v[1] = std::numeric_limits<double>::max();
		v[2] = -std::numeric_limits<double>::max();
		return false;
	}

	bool ProjectorPerspective::Backward(glm::dvec3& v) const
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
