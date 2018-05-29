SET(LIB_NAME "SkyRendererCore")

SET(SAURON_SKY_RENDERER_DIR "${SAURON_ROOT_DIR}/ConceptValidation/SkyRenderer")
SET(SAURON_SKY_RENDERER_OUTPUT_DIR "${SAURON_SKY_RENDERER_DIR}/Lib/${SAURON_PLATFORM_NAME}")

SET(CORE_HEADER_FILES
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Context.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Core.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/DrawEntity.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/ErrorHandling.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/GraphicsSystem.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Location.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/LocationManager.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Module.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/ModuleManager.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Observer.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Painter.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Projector.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/RefractionExtinction.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/SkyDrawer.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Texture.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Include/Sauron/Util.hpp
)
SET(CORE_SOURCE_FILES
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Context.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Core.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/ErrorHandling.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/GraphicsSystem.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Location.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/LocationManager.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/ModuleManager.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Observer.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Painter.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Projector.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/RefractionExtinction.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/SkyDrawer.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Texture.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Util.cpp
)

SET(CORE_MODULES_HREADER_FILES
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Modules/MilkyWay.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Modules/SolarSystem.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Modules/Planet.hpp
)
SET(CORE_MODULES_SOURCE_FILES
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Modules/MilkyWay.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Modules/Planet.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/Modules/SolarSystem.cpp
)

SET(CORE_PLANETS_EPHEMS_HREADER_FILES
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/calc_interpolated_elements.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/de430.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/de431.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/elliptic_to_rectangular.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/elp82b.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/EphemWrapper.hpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/gust86.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/jpl_int.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/jpleph.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/l1.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/marssat.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/pluto.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/precession.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/sidereal_time.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/tass17.h
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/vsop87.h
)
SET(CORE_PLANETS_EPHEMS_SOURCE_FILES
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/calc_interpolated_elements.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/de430.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/de431.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/elliptic_to_rectangular.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/elp82b.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/EphemWrapper.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/gust86.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/jpleph.cpp
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/l1.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/marssat.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/pluto.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/precession.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/sidereal_time.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/tass17.c
	${SAURON_SKY_RENDERER_DIR}/Core/Src/PlanetsEphems/vsop87.c
)

SOURCE_GROUP("Core\\Source Files" FILES ${CORE_SOURCE_FILES})
SOURCE_GROUP("Core\\Header Files" FILES ${CORE_HEADER_FILES})
SOURCE_GROUP("Core\\Source Files\\Modules" FILES ${CORE_MODULES_SOURCE_FILES})
SOURCE_GROUP("Core\\Header Files\\Modules" FILES ${CORE_MODULES_HREADER_FILES})
SOURCE_GROUP("Core\\Source Files\\PlanetsEphems" FILES ${CORE_PLANETS_EPHEMS_SOURCE_FILES})
SOURCE_GROUP("Core\\Header Files\\PlanetsEphems" FILES ${CORE_PLANETS_EPHEMS_HREADER_FILES})

ADD_DEFINITIONS(-DSAURON_SKY_RENDERER_CORE_SOURCE)
ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
IF(CMAKE_C_COMPILER_ID MATCHES MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201")
ENDIF()

INCLUDE_DIRECTORIES(${SAURON_ROOT_DIR}/External/glm)
INCLUDE_DIRECTORIES(${SAURON_ROOT_DIR}/External/boost)
INCLUDE_DIRECTORIES(${SAURON_ROOT_DIR}/External/angle/include)
INCLUDE_DIRECTORIES(${SAURON_ROOT_DIR}/External/stb)
INCLUDE_DIRECTORIES(${SAURON_SKY_RENDERER_DIR}/Core/Include)
LINK_DIRECTORIES(${SAURON_ROOT_DIR}/External/angle/lib/${SAURON_PLATFORM_NAME})
ADD_LIBRARY(${LIB_NAME} STATIC
	${CORE_SOURCE_FILES} ${CORE_HEADER_FILES}
	${CORE_MODULES_SOURCE_FILES} ${CORE_MODULES_HREADER_FILES}
	${CORE_PLANETS_EPHEMS_SOURCE_FILES} ${CORE_PLANETS_EPHEMS_HREADER_FILES}
)

SET(LIB_OUTPUT_NAME ${LIB_NAME}${SAURON_OUTPUT_SUFFIX})

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${SAURON_SKY_RENDERER_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SAURON_SKY_RENDERER_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SAURON_SKY_RENDERER_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SAURON_SKY_RENDERER_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${SAURON_SKY_RENDERER_OUTPUT_DIR}
	PROJECT_LABEL ${LIB_NAME}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
	OUTPUT_NAME ${LIB_OUTPUT_NAME}
)

TARGET_LINK_LIBRARIES(${LIB_NAME} libGLESv2 libEGL)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES FOLDER "ConceptValidation/SkyRenderer")
