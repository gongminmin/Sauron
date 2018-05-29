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

#include <Sauron/Core.hpp>

#include <algorithm>
#include <chrono>

#include <glm/gtc/matrix_transform.hpp>

#include <Sauron/Context.hpp>
#include <Sauron/Location.hpp>
#include <Sauron/LocationManager.hpp>
#include <Sauron/Observer.hpp>
#include <Sauron/Projector.hpp>
#include <Sauron/RefractionExtinction.hpp>
#include <Sauron/Util.hpp>

#include "Modules/SolarSystem.hpp"

namespace
{
	//! Compute date in decimal year format
	//! @param year
	//! @param month
	//! @param day
	//! @return decimal year
	double GetDecYear(int year, int month, int day)
	{
		return year + ((month - 1) * 30.5 + day / 31.0 * 30.5) / 366;
	}

	//! Make from julian day a year, month, day for the Julian Date julian day represents.
	void GetDateFromJulianDay(double jd, int* yy, int* mm, int* dd)
	{
		/*
		 * This algorithm is taken from
		 * "Numerical Recipes in c, 2nd Ed." (1992), pp. 14-15
		 * and converted to integer math.
		 * The electronic version of the book is freely available
		 * at http://www.nr.com/ , under "Obsolete Versions - Older
		 * book and code versions.
		 */

		static long constexpr JD_GREG_CAL = 2299161;
		static int constexpr JB_MAX_WITHOUT_OVERFLOW = 107374182;

		int32_t julian = static_cast<int32_t>(floor(jd + 0.5));

		int32_t ta, jalpha, tb, tc, td, te;

		if (julian >= JD_GREG_CAL)
		{
			jalpha = (4 * (julian - 1867216) - 1) / 146097;
			ta = julian + 1 + jalpha - jalpha / 4;
		}
		else if (julian < 0)
		{
			ta = julian + 36525 * (1 - julian / 36525);
		}
		else
		{
			ta = julian;
		}

		tb = ta + 1524;
		if (tb <= JB_MAX_WITHOUT_OVERFLOW)
		{
			tc = (tb * 20 - 2442) / 7305;
		}
		else
		{
			tc = static_cast<int32_t>((static_cast<uint64_t>(tb * 20) - 2442) / 7305);
		}
		td = 365 * tc + tc / 4;
		te = ((tb - td) * 10000) / 306001;

		*dd = tb - td - (306001 * te) / 10000;

		*mm = te - 1;
		if (*mm > 12)
		{
			*mm -= 12;
		}
		*yy = tc - 4715;
		if (*mm > 2)
		{
			-- (*yy);
		}
		if (julian < 0)
		{
			*yy -= 100 * (1 - julian / 36525);
		}
	}

	//! Get Delta-T estimation for a given date.
	//! Note that this method is recommended for the year range:
	//! -1999 to +3000. It gives details for -500...+2150.
	//! Implementation of algorithm by Espenak & Meeus (2006) for DeltaT computation
	//! @param jd the date and time expressed as a Julian day
	//! @return Delta-T in seconds
	double GetDeltaTByEspenakMeeus(double jd)
	{
		int year, month, day;
		GetDateFromJulianDay(jd, &year, &month, &day);

		// Note: the method here is adapted from
		// "Five Millennium Canon of Solar Eclipses" [Espenak and Meeus, 2006]
		// A summary is described here:
		// http://eclipse.gsfc.nasa.gov/SEhelp/deltatpoly2004.html

		double y = GetDecYear(year, month, day);

		// set the default value for Delta T
		double u = (y - 1820) / 100;
		double r = (-20 + 32 * u * u);

		if (y < -500)
		{
			// values are equal to defaults!
			// u = (y - 1820) / 100.0;
			// r = (-20 + 32 * u * u);
		}
		else if (y < 500)
		{
			u = y / 100;
			//r = (10583.6 - 1014.41 * u + 33.78311 * std::pow(u,2) - 5.952053 * std::pow(u,3)
			//       - 0.1798452 * std::pow(u,4) + 0.022174192 * std::pow(u,5) + 0.0090316521 * std::pow(u,6));
			r = (((((0.0090316521 * u + 0.022174192) * u - 0.1798452) * u - 5.952053) * u + 33.78311) * u - 1014.41) * u + 10583.6;
		}
		else if (y < 1600)
		{
			u = (y - 1000) / 100;
			//r = (1574.2 - 556.01 * u + 71.23472 * std::pow(u, 2) + 0.319781 * std::pow(u, 3)
			//       - 0.8503463 * std::pow(u,4) - 0.005050998 * std::pow(u,5) + 0.0083572073 * std::pow(u, 6));
			r = (((((0.0083572073 * u - 0.005050998) * u - 0.8503463) * u + 0.319781) * u + 71.23472) * u - 556.01) * u + 1574.2;
		}
		else if (y < 1700)
		{
			auto t = y - 1600;
			//r = (120 - 0.9808 * t - 0.01532 * std::pow(t,2) + std::pow(t,3) / 7129);
			r = ((t / 7129.0 - 0.01532) * t - 0.9808) * t + 120.0;
		}
		else if (y < 1800)
		{
			auto t = y - 1700;
			//r = (8.83 + 0.1603 * t - 0.0059285 * std::pow(t,2) + 0.00013336 * std::pow(t,3) - std::pow(t,4) / 1174000);
			r = (((-t / 1174000.0 + 0.00013336) * t - 0.0059285) * t + 0.1603) * t + 8.83;
		}
		else if (y < 1860)
		{
			auto t = y - 1800;
			//r = (13.72 - 0.332447 * t + 0.0068612 * std::pow(t,2) + 0.0041116 * std::pow(t,3) - 0.00037436 * std::pow(t,4)
			//       + 0.0000121272 * std::pow(t,5) - 0.0000001699 * std::pow(t,6) + 0.000000000875 * std::pow(t,7));
			r = ((((((0.000000000875 * t - .0000001699) * t + 0.0000121272) * t - 0.00037436) * t + 0.0041116)
				* t + 0.0068612) * t - 0.332447) * t + 13.72;
		}
		else if (y < 1900)
		{
			auto t = y - 1860;
			//r = (7.62 + 0.5737 * t - 0.251754 * std::pow(t,2) + 0.01680668 * std::pow(t,3)
			//	-0.0004473624 * std::pow(t,4) + std::pow(t,5) / 233174);
			r = ((((t / 233174.0 - 0.0004473624) * t + 0.01680668) * t - 0.251754) * t + 0.5737) * t + 7.62;
		}
		else if (y < 1920)
		{
			auto t = y - 1900;
			//r = (-2.79 + 1.494119 * t - 0.0598939 * std::pow(t,2) + 0.0061966 * std::pow(t,3) - 0.000197 * std::pow(t,4));
			r = (((-0.000197 * t + 0.0061966) * t - 0.0598939) * t + 1.494119) * t - 2.79;
		}
		else if (y < 1941)
		{
			auto t = y - 1920;
			//r = (21.20 + 0.84493*t - 0.076100 * std::pow(t,2) + 0.0020936 * std::pow(t,3));
			r = ((0.0020936 * t - 0.076100) * t + 0.84493) * t + 21.20;
		}
		else if (y < 1961)
		{
			auto t = y - 1950;
			//r = (29.07 + 0.407*t - std::pow(t,2)/233 + std::pow(t,3) / 2547);
			r = ((t / 2547.0 - 1 / 233.0) * t + 0.407) * t + 29.07;
		}
		else if (y < 1986)
		{
			auto t = y - 1975;
			//r = (45.45 + 1.067*t - std::pow(t,2)/260 - std::pow(t,3) / 718);
			r = ((-t / 718.0 - 1 / 260.0) * t + 1.067) * t + 45.45;
		}
		else if (y < 2005)
		{
			auto t = y - 2000;
			//r = (63.86 + 0.3345 * t - 0.060374 * std::pow(t,2) + 0.0017275 * std::pow(t,3) + 0.000651814 * std::pow(t,4) + 0.00002373599 * std::pow(t,5));
			r = ((((0.00002373599 * t + 0.000651814) * t + 0.0017275) * t - 0.060374) * t + 0.3345) * t + 63.86;
		}
		else if (y < 2050)
		{
			auto t = y - 2000;
			//r = (62.92 + 0.32217 * t + 0.005589 * std::pow(t,2));
			r = (0.005589 * t + 0.32217) * t + 62.92;
		}
		else if (y < 2150)
		{
			//r = (-20 + 32 * std::pow((y-1820)/100,2) - 0.5628 * (2150 - y));
			// r has been precomputed before, just add the term patching the discontinuity
			r -= 0.5628 * (2150 - y);
		}

		return r;
	}
	
	//! Get Secular Acceleration estimation for a given year.
	//! Method described is here: http://eclipse.gsfc.nasa.gov/SEcat5/secular.html
	//! For adapting from -26 to -25.858, use -0.91072 * (-25.858 + 26.0) = -0.12932224
	//! For adapting from -26 to -23.895, use -0.91072 * (-23.895 + 26.0) = -1.9170656
	//! @param jd the JD
	//! @param n_dot value of n-dot (secular acceleration of the Moon) which should be used in the lunar ephemeris instead of the default values.
	//! @param use_de43x true if function should adapt calculation of the secular acceleration of the Moon to the DE43x ephemeris
	//! @return SecularAcceleration in seconds
	//! @note n-dot for secular acceleration of the Moon in ELP2000-82B is -23.8946 "/cy/cy and for DE43x is -25.8 "/cy/cy
	double GetMoonSecularAcceleration(double jd, double n_dot, bool use_de43x)
	{
		int year, month, day;
		GetDateFromJulianDay(jd, &year, &month, &day);

		double t = (GetDecYear(year, month, day) - 1955.5) / 100;
		// n.dot for secular acceleration of the Moon in ELP2000-82B
		// have value -23.8946 "/cy/cy (or -25.8 for DE43x usage)
		double eph_nd = use_de43x ? -25.8 : -23.8946;

		return -0.91072 * (eph_nd + std::abs(n_dot)) * t * t;
	}
}

namespace Sauron
{
	Core::Core()
	{
	}

	void Core::Init()
	{
		auto& location_mgr = Context::GetInstance().GetLocationManager();
		auto location = location_mgr.GetLastLocation();
		location_mgr.LocationFromSystem();
		curr_observer_ = std::make_shared<Observer>(location);

		using namespace std::chrono_literals;
		auto now = std::chrono::system_clock::now();
		this->SetJD((std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() + 58574100h).count()) / 1000.0 * JD_SECOND);

		this->UpdateTransformMatrices();

		// TODO
		glm::dvec3 view_direction_j2000 = this->AltAzToJ2000(glm::dvec3(1, 0, 0), RefractionMode::Off);
		glm::dvec3 up_direction_j2000 = this->AltAzToJ2000(glm::dvec3(0, 0, 1), RefractionMode::Off);
		this->LookAtJ2000(view_direction_j2000, up_direction_j2000);

		sky_drawer_ = std::make_unique<SkyDrawer>(this);
	}

	void Core::Update(double delta_time)
	{
		// TODO

		this->UpdateTime(delta_time);

		this->UpdateTransformMatrices();

		sky_drawer_->Update(delta_time);
	}

	void Core::WindowHasBeenResized(float x, float y, float width, float height)
	{
		// Maximize display when resized since it invalidates previous options anyway
		curr_projector_params_.viewport_xy_wh = glm::vec4(x, y, width, height);
		curr_projector_params_.viewport_center = glm::vec2(x + width * 0.5f, y + height * 0.5f);
		curr_projector_params_.viewport_fov_diameter = std::min(width, height);
	}

	void Core::PreDraw()
	{
		// Init openGL viewing with fov, screen size and clip planes
		curr_projector_params_.z_near = 0.000001f;
		curr_projector_params_.z_far = 500;
	}

	void Core::PostDraw()
	{
		// TODO
	}

	void Core::Draw()
	{
		// TODO
	}

	void Core::SetCurrentObserver(std::shared_ptr<Observer> const & ob)
	{
		curr_observer_ = ob;
	}

	std::shared_ptr<Observer> const & Core::GetCurrentObserver() const
	{
		return curr_observer_;
	}

	void Core::MoveObserverTo(Location const & target)
	{
		this->SetCurrentObserver(std::make_shared<Observer>(target));
		// TODO: Send signals about target changed and location changed
	}

	std::shared_ptr<Projector> Core::GetProjection(std::shared_ptr<Projector::ModelViewTransform> const & transform) const
	{
		auto ret = std::make_shared<ProjectorPerspective>(transform);
		ret->Init(curr_projector_params_);
		return ret;
	}

	Projector::ProjectorParams Core::GetCurrentProjectorParams() const
	{
		return curr_projector_params_;
	}

	void Core::SetCurrentProjectorParams(Projector::ProjectorParams const & params)
	{
		curr_projector_params_ = params;
	}

	SkyDrawer* Core::GetSkyDrawer()
	{
		return sky_drawer_.get();
	}

	SkyDrawer const * Core::GetSkyDrawer() const
	{
		return sky_drawer_.get();
	}

	void Core::LookAtJ2000(glm::dvec3 const & pos, glm::dvec3 const & up_vec)
	{
		auto forward = this->J2000ToAltAz(pos, RefractionMode::Off);
		auto up = this->J2000ToAltAz(up_vec, RefractionMode::Off);
		forward = glm::normalize(forward);
		up = glm::normalize(up);

		// Update the model view matrix
		auto right = glm::cross(forward, up);	// y vector
		right = glm::normalize(right);
		up = glm::cross(right, forward);	// Up vector in AltAz coordinates
		up = glm::normalize(up);
		mat_alt_az_model_view_ = glm::dmat4(right.x, up.x, -forward.x, 0,
			right.y, up.y, -forward.y, 0,
			right.z, up.z, -forward.z, 0,
			0, 0, 0, 1);
		invert_mat_alt_az_model_view_ = glm::inverse(mat_alt_az_model_view_);
	}

	glm::dvec3 Core::AltAzToJ2000(glm::dvec3 const & v, RefractionMode ref_mode) const
	{
		if ((ref_mode == RefractionMode::Off)
			|| !sky_drawer_
			|| ((ref_mode == RefractionMode::Auto) && !sky_drawer_->ShowAtmosphere()))
		{
			return mat_equinox_equ_to_j2000_ * mat_alt_az_to_equinox_equ_ * glm::dvec4(v, 1);
		}

		glm::dvec3 r = v;
		sky_drawer_->GetRefraction().Backward(r);
		return mat_equinox_equ_to_j2000_ * mat_alt_az_to_equinox_equ_ * glm::dvec4(r, 1);
	}

	glm::dvec3 Core::J2000ToAltAz(glm::dvec3 const & v, RefractionMode ref_mode) const
	{
		if ((ref_mode == RefractionMode::Off)
			|| !sky_drawer_
			|| ((ref_mode == RefractionMode::Auto) && !sky_drawer_->ShowAtmosphere()))
		{
			return mat_j2000_to_alt_az_ * glm::dvec4(v, 1);
		}

		glm::dvec3 r = mat_j2000_to_alt_az_ * glm::dvec4(v, 1);
		sky_drawer_->GetRefraction().Forward(r);
		return r;
	}

	std::shared_ptr<Projector::ModelViewTransform> Core::GetJ2000ModelViewTransform(RefractionMode ref_mode) const
	{
		if ((ref_mode == RefractionMode::Off)
			|| !sky_drawer_
			|| ((ref_mode == RefractionMode::Auto) && !sky_drawer_->ShowAtmosphere()))
		{
			return std::make_shared<Projector::Mat4Transform>(
				mat_alt_az_model_view_ * mat_equinox_equ_to_alt_az_ * mat_j2000_to_equinox_equ_);
		}

		std::shared_ptr<Refraction> refr = std::make_shared<Refraction>(sky_drawer_->GetRefraction());
		// The pretransform matrix will convert from input coordinates to AltAz needed by the refraction function.
		refr->SetPreTransformMatrix(mat_equinox_equ_to_alt_az_ * mat_j2000_to_equinox_equ_);
		refr->SetPostTransformMatrix(mat_alt_az_model_view_);
		return refr;
	}

	void Core::UpdateTransformMatrices()
	{
		mat_alt_az_to_equinox_equ_ = curr_observer_->GetRotAltAzToEquatorial(this->GetJD(), this->GetJDE());
		mat_equinox_equ_to_alt_az_ = glm::transpose(mat_alt_az_to_equinox_equ_);

		// multiply static J2000 earth axis tilt (eclipticalJ2000<->equatorialJ2000)
		// in effect, this matrix transforms from VSOP87 ecliptical J2000 to planet-based equatorial coordinates.
		// For earth, mat_j2000_to_equinox_equ_ is the precession matrix.
		// TODO: rename mat_equinox_equ_to_j2000_ to mat_equinox_equ_date_to_j2000_
		mat_equinox_equ_to_j2000_ = this->GetMatVsop87ToJ2000() * curr_observer_->GetRotEquatorialToVsop87();
		mat_j2000_to_equinox_equ_ = glm::transpose(mat_equinox_equ_to_j2000_);
		mat_j2000_to_alt_az_ = mat_equinox_equ_to_alt_az_ * mat_j2000_to_equinox_equ_;
		mat_alt_az_to_j2000_ = glm::transpose(mat_j2000_to_alt_az_);

		mat_heliocentric_ecliptic_to_equinox_equ_ = mat_j2000_to_equinox_equ_ * this->GetMatVsop87ToJ2000()
			* glm::translate(glm::dmat4(1), -curr_observer_->GetCenterVsop87Pos());

		// These two next have to take into account the position of the observer on the earth/planet of observation.
		// GZ tmp could be called matAltAzToVsop87
		glm::dmat4 tmp = this->GetMatJ2000ToVsop87() * mat_equinox_equ_to_j2000_ * mat_alt_az_to_equinox_equ_;
		//glm::dmat4 tmp1 = mat_j2000_to_vsop87_ * mat_equinox_equ_to_j2000_;

		// Before 0.14 GetDistanceFromCenter assumed spherical planets. Now uses rectangular coordinates for observer!
		// In series 0.14 and 0.15, this was erroneous: offset by distance rho, but in the wrong direction.
		// Attempt to fix LP:1275092. This improves the situation, but is still not perfect.
		// Please keep the commented stuff until situation is really solved.
		if (use_topocentric_coordinates_)
		{
			glm::dvec3 offset = curr_observer_->GetTopographicOffsetFromCenter(); // [rho cosPhi', rho sinPhi', phi'_rad]
			double const sigma = Deg2Rad(curr_observer_->GetCurrentLocation().GetLatitude()) - offset.y;
			double const rho = curr_observer_->GetDistanceFromCenter();

			mat_alt_az_to_heliocentric_ecliptic_j2000_ = glm::translate(glm::dmat4(1), curr_observer_->GetCenterVsop87Pos()) * tmp
				* glm::translate(glm::dmat4(1), glm::dvec3(rho * sin(sigma), 0, rho * cos(sigma)));

			mat_heliocentric_ecliptic_j2000_to_alt_az_
				= glm::translate(glm::dmat4(1), glm::dvec3(-rho * sin(sigma), 0., -rho * cos(sigma)))
					* glm::transpose(tmp) * glm::translate(glm::dmat4(1), -curr_observer_->GetCenterVsop87Pos());

			// Here I tried to split tmp matrix. This does not work:
			//		mat_alt_az_to_heliocentric_ecliptic_j2000_ =  glm::translate(glm::dmat4(1), curr_observer_->GetCenterVsop87Pos()) * tmp1 *
			//				glm::translate(glm::dvec3(rho * sin(sigma), 0., rho * cos(sigma) )) * mat_alt_az_to_equinox_equ_;

			//		mat_heliocentric_ecliptic_j2000_to_alt_az_ =
			//				mat_equinox_equ_to_alt_az_ *
			//				glm::translate(glm::dmat4(1), Vec3d(-rho * sin(sigma), 0., -rho * cos(sigma))) * glm::transpose(tmp1) *
			//				glm::translate(glm::dmat4(1), -curr_observer_->GetCenterVsop87Pos());


			//		mat_alt_az_to_heliocentric_ecliptic_j2000_ =  glm::translate(glm::dmat4(1), curr_observer_->GetCenterVsop87Pos()) * tmp *
			//				glm::translate(glm::dmat4(1), glm::dvec3(0, 0, curr_observer_->GetDistanceFromCenter()));

			//		mat_heliocentric_ecliptic_j2000_to_alt_az_ =  Mat4d::translation(glm::dvec3(0, 0, -curr_observer_->GetDistanceFromCenter())) * glm::transpose(tmp) *
			//				glm::translate(glm::dmat4(1), -curr_observer_->GetCenterVsop87Pos());
		}
		else
		{
			mat_alt_az_to_heliocentric_ecliptic_j2000_ = glm::translate(glm::dmat4(1), curr_observer_->GetCenterVsop87Pos()) * tmp;
			mat_heliocentric_ecliptic_j2000_to_alt_az_ = glm::transpose(tmp)
				* glm::translate(glm::dmat4(1), -curr_observer_->GetCenterVsop87Pos());
		}
	}

	void Core::UpdateTime(double delta_time)
	{
		SAURON_UNUSED(delta_time);

		auto now = std::chrono::system_clock::now();
		julian_day_.first = jd_of_last_jd_update_
			+ (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
				- milli_seconds_of_last_jd_update_) / 1000.0 * JD_SECOND;

		// Fix time limits to -100000 to +100000 to prevent bugs
		if (julian_day_.first > 38245309.499988f)
		{
			julian_day_.first = 38245309.499988f;
		}
		if (julian_day_.first < -34803211.500012f)
		{
			julian_day_.first = -34803211.500012f;
		}
		julian_day_.second = this->ComputeDeltaT(julian_day_.first);

		// TODO
		// Position of sun and all the satellites (ie planets)
		static auto* sol_system = GET_MODULE(SolarSystem);
		// Likely the most important location where we need JDE:
		sol_system->ComputePositions(this->GetJDE(), curr_observer_->GetHomePlanet());
	}

	void Core::ResetSync()
	{
		jd_of_last_jd_update_ = this->GetJD();

		auto now = std::chrono::system_clock::now();
		milli_seconds_of_last_jd_update_ = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	}

	glm::dmat4 const & Core::GetMatJ2000ToVsop87()
	{
		static glm::dmat4 const mat_j2000_to_vsop87 = glm::rotate(glm::dmat4(1), Deg2Rad(-23.4392803055555555556), glm::dvec3(1, 0, 0))
			* glm::rotate(glm::dmat4(1), Deg2Rad(0.0000275), glm::dvec3(0, 0, 1));
		return mat_j2000_to_vsop87;
	}

	glm::dmat4 const & Core::GetMatVsop87ToJ2000()
	{
		static glm::dmat4 const mat_vsop87_to_j2000 = glm::transpose(Core::GetMatJ2000ToVsop87());
		return mat_vsop87_to_j2000;
	}

	double Core::GetJD() const
	{
		return julian_day_.first;
	}

	void Core::SetJD(double new_jd)
	{
		julian_day_.first = new_jd;
		julian_day_.second = this->ComputeDeltaT(new_jd);
		this->ResetSync();
	}

	double Core::GetJDE() const
	{
		return julian_day_.first + julian_day_.second / (24 * 60 * 60);
	}

	void Core::SetJDE(double new_jde)
	{
		// nitpickerish this is not exact, but as good as it gets...
		julian_day_.second = this->ComputeDeltaT(new_jde);
		julian_day_.first = new_jde - julian_day_.second / (24 * 60 * 60);
		this->ResetSync();
	}

	double Core::ComputeDeltaT(double jd)
	{
		// Espenak & Meeus (2006) algorithm for DeltaT
		double constexpr delta_t_n_dot = -25.858; // n.dot = -25.858 "/cy/cy

		double delta_t = GetDeltaTByEspenakMeeus(jd);
		delta_t += GetMoonSecularAcceleration(jd, delta_t_n_dot, false);

		return delta_t;
	}
}
