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

#ifndef SAURON_SKY_RENDERER_CORE_SKY_DRAWER_HPP
#define SAURON_SKY_RENDERER_CORE_SKY_DRAWER_HPP

#pragma once

#include <Sauron/RefractionExtinction.hpp>

namespace Sauron
{
	class Core;

	//! @class SkyDrawer
	//! Provide a set of methods used to draw sky objects taking into account
	//! eyes adaptation, zoom level, instrument model and artificially set magnitude limits
	class SkyDrawer
	{
	public:
		SkyDrawer(Core* core);
		~SkyDrawer();

		//! Init parameters from config file
		void Init();

		//! Update with respect to the time and StelProjector/StelToneReproducer state
		//! @param deltaTime the time increment in second since last call.
		void Update(double delta_time);

		//! Compute the luminance for an extended source with the given surface brightness
		//! @param sb surface brightness in V magnitude/arcmin^2
		//! @return the luminance in cd/m^2
		static float SurfaceBrightnessToLuminance(float sb)
		{
			return 2 * 2025000 * std::exp(-0.92103f * (sb + 12.12331f)) / (1 / 3600.0f);
		}
		//! Compute the surface brightness from the luminance of an extended source
		//! @param lum luminance in cd/m^2
		//! @return surface brightness in V magnitude/arcmin^2
		static float LuminanceToSurfacebrightness(float lum)
		{
			return std::log(lum * (1 / 3600.0f) / (2 * 2025000)) / -0.92103f - 12.12331f;
		}

		//! Get the parameters so that the stars disappear at about the limit given by the bortle scale
		//! @see https://en.wikipedia.org/wiki/Bortle_scale
		int GetBortleScaleIndex() const
		{
			return bortle_scale_index_;
		}
		//! Set the current Bortle scale index
		//! The limit is valid only at a given zoom level (around 60 deg)
		void SetBortleScaleIndex(int index);
		//! Get the average NELM for current Bortle scale index
		float GetNELMFromBortleScale() const;

		//! Informing the drawer whether atmosphere is displayed.
		//! This is used to avoid twinkling/simulate extinction/refraction.
		void ShowAtmosphere(bool sa)
		{
			show_atmosphere_ = sa;
		}

		//! This is used to decide whether to apply refraction/extinction before rendering point sources et al.
		bool ShowAtmosphere() const
		{
			return show_atmosphere_;
		}

		//! Get extinction coefficient, mag/airmass (for extinction).
		double GetExtinctionCoefficient() const
		{
			return extinction_.GetExtinctionCoefficient();
		}
		//! Set extinction coefficient, mag/airmass (for extinction).
		void SetExtinctionCoefficient(float ext_coeff)
		{
			extinction_.SetExtinctionCoefficient(ext_coeff);
		}
		//! Get atmospheric (ground) temperature in deg celsius (for refraction).
		double GetAtmosphereTemperature() const
		{
			return refraction_.GetTemperature();
		}
		//! Set atmospheric (ground) temperature in deg celsius (for refraction).
		void SetAtmosphereTemperature(float celsius)
		{
			refraction_.SetTemperature(celsius);
		}
		//! Get atmospheric (ground) pressure in mbar (for refraction).
		double GetAtmospherePressure() const
		{
			return refraction_.GetPressure();
		}
		//! Set atmospheric (ground) pressure in mbar (for refraction).
		void SetAtmospherePressure(float mbar)
		{
			refraction_.SetPressure(mbar);
		}

		//! Get the current valid extinction computation object.
		Extinction const & GetExtinction() const
		{
			return extinction_;
		}
		//! Get the current valid refraction computation object.
		Refraction const & GetRefraction() const
		{
			return refraction_;
		}

	private:
		Core* core_;

		Extinction extinction_;
		Refraction refraction_;

		bool show_atmosphere_;

		//! The current Bortle Scale index
		int bortle_scale_index_ = 2;
	};
}

#endif		// SAURON_SKY_RENDERER_CORE_PROJECTOR_HPP
