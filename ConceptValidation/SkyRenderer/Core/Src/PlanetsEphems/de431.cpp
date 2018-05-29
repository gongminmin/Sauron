/*
Copyright (c) 2015 Holger Niessner
Copyright (c) 2016 Georg Zotti

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "de431.hpp"
#include "jpleph.h"
#include <Sauron/Core.hpp>

#include <glm/vec3.hpp>

#include <iostream>

extern "C" {

static void * ephem;
   
static glm::dvec3 tempECLpos = glm::dvec3(0,0,0);
static glm::dvec3 tempECLspd = glm::dvec3(0,0,0);
static glm::dvec3 tempICRFpos = glm::dvec3(0,0,0);
static glm::dvec3 tempICRFspd = glm::dvec3(0,0,0);
static char nams[JPL_MAX_N_CONSTANTS][6];
static double vals[JPL_MAX_N_CONSTANTS];
static double tempXYZ[6];
#ifdef UNIT_TEST
// NOTE: Added hook for unit testing
static const Mat4d matJ2000ToVsop87(Mat4d::xrotation(-23.4392803055555555556*(M_PI/180)) * Mat4d::zrotation(0.0000275*(M_PI/180)));
#endif

static bool initDone = false;

void InitDE431(const char* filepath)
{
	ephem = jpl_init_ephemeris(filepath, nams, vals);

	if(jpl_init_error_code() != 0)
	{
		std::cerr << "Error "<< jpl_init_error_code() << "at DE431 init:" << jpl_init_error_message() << std::endl;
	}
	else
	{
		initDone = true;
		double jd1, jd2;
		jd1=jpl_get_double(ephem, JPL_EPHEM_START_JD);
		jd2=jpl_get_double(ephem, JPL_EPHEM_END_JD);
		std::cerr << "[DEBUG] DE431 init successful. startJD=" << jd1 << "endJD=" << jd2 << std::endl;
	}
}

void TerminateDE431()
{
  jpl_close_ephemeris(ephem);
}

bool GetDe431Coor(const double jde, const int planet_id, double * xyz, const int centralBody_id)
{
	if(initDone)
	{
	// This may return some error code!
	int jplresult=jpl_pleph(ephem, jde, planet_id, centralBody_id, tempXYZ, 1);

	switch (jplresult)
	{
		case 0: // all OK.
			break;
		case JPL_EPH_OUTSIDE_RANGE:
			std::cerr << "[DEBUG] GetDe431Coor: JPL_EPH_OUTSIDE_RANGE at jde" << jde << "for planet" << planet_id << std::endl;
			return false;
			break;
		case JPL_EPH_READ_ERROR:
			std::cerr << "[DEBUG] GetDe431Coor: JPL_EPH_READ_ERROR at jde" << jde << "for planet" << planet_id << std::endl;
			return false;
			break;
		case JPL_EPH_QUANTITY_NOT_IN_EPHEMERIS:
			std::cerr << "[DEBUG] GetDe431Coor: JPL_EPH_QUANTITY_NOT_IN_EPHEMERIS at jde" << jde << "for planet" << planet_id << std::endl;
			return false;
			break;
		case JPL_EPH_INVALID_INDEX:
			std::cerr << "[DEBUG] GetDe431Coor: JPL_EPH_INVALID_INDEX at jde" << jde << "for planet" << planet_id << std::endl;
			return false;
			break;
		case JPL_EPH_FSEEK_ERROR:
			std::cerr << "[DEBUG] GetDe431Coor: JPL_EPH_FSEEK_ERROR at jde" << jde << "for planet" << planet_id << std::endl;
			return false;
			break;
		default: // Should never happen...
			std::cerr << "[DEBUG] GetDe431Coor: unknown error" << jplresult << "at jde" << jde << "for planet" << planet_id << std::endl;
			return false;
			break;
	}

	tempICRFpos = glm::dvec3(tempXYZ[0], tempXYZ[1], tempXYZ[2]);
	tempICRFspd = glm::dvec3(tempXYZ[3], tempXYZ[4], tempXYZ[5]);
	glm::dvec4 tmp = Sauron::Core::GetMatJ2000ToVsop87() * glm::dvec4(tempICRFpos, 1);
	tempECLpos = glm::dvec3(tmp.x, tmp.y, tmp.z);
	tmp = Sauron::Core::GetMatJ2000ToVsop87() * glm::dvec4(tempICRFspd, 1);
	tempECLspd = glm::dvec3(tmp.x, tmp.y, tmp.z);

	xyz[0] = tempECLpos[0];
	xyz[1] = tempECLpos[1];
	xyz[2] = tempECLpos[2];
	xyz[3] = tempECLspd[0];
	xyz[4] = tempECLspd[1];
	xyz[5] = tempECLspd[2];
	return true;
	}
	return false;
}

}
