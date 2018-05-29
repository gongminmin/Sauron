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

#ifndef SAURON_SKY_RENDERER_CORE_ERROR_HANDLING_HPP
#define SAURON_SKY_RENDERER_CORE_ERROR_HANDLING_HPP

#pragma once

#include <Sauron/Util.hpp>

#if defined(_MSC_VER)
	#define SAURON_ATTRIBUTE_NORETURN __declspec(noreturn)
	#define SAURON_BUILTIN_UNREACHABLE __assume(false)
#else
	#define SAURON_ATTRIBUTE_NORETURN __attribute__((noreturn))
	#define SAURON_BUILTIN_UNREACHABLE __builtin_unreachable()
#endif

namespace Sauron
{
#if defined(SAURON_DEBUG) || !defined(SAURON_BUILTIN_UNREACHABLE)
	SAURON_ATTRIBUTE_NORETURN void UnreachableInternal(char const * msg = nullptr, char const * file = nullptr, uint32_t line = 0);
#endif
}

#ifdef SAURON_DEBUG
	#define SAURON_UNREACHABLE(msg) Sauron::UnreachableInternal(msg, __FILE__, __LINE__)
#elif defined(SAURON_BUILTIN_UNREACHABLE)
	#define SAURON_UNREACHABLE(msg) SAURON_BUILTIN_UNREACHABLE
#else
	#define SAURON_UNREACHABLE(msg) Sauron::UnreachableInternal()
#endif

#endif		// SAURON_SKY_RENDERER_CORE_ERROR_HANDLING_HPP
