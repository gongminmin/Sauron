# - Try to find OpenGL ES
# Once done this will define
#  
#  SAURON_GLES_FOUND       - system has OpenGL ES
#  SAURON_GLES_INCLUDE_DIR - Directory to the OpenGL ES SDK's include
#   

SET(ProgramFilesX86 "ProgramFiles(x86)")

IF(NOT SAURON_GLES_INCLUDE_DIR)
	IF(WIN32)
		FIND_PATH(SAURON_GLES_INCLUDE_DIR KHR/khrplatform.h
			PATHS
				"$ENV{SystemDrive}/AdrenoSDK/Development/Inc"
				"$ENV{NV_WINGL_X86_PLAT}/include"
				"$ENV{ProgramFiles}/NVIDIA Corporation/win_x86_es2emu/include"
				"$ENV{${ProgramFilesX86}}/NVIDIA Corporation/win_x86_es2emu/include"
				"$ENV{SystemDrive}/Imagination/PowerVR/GraphicsSDK/SDK_*/Builds/include"
				"$ENV{SystemDrive}/Imagination/PowerVR_Graphics/PowerVR_SDK/SDK_*/Builds/include"
				"$ENV{KHRONOS_HEADERS}"
				"$ENV{ProgramFiles}/ARM/Mali Developer Tools/Mali OpenGL ES Emulator*/include"
				"$ENV{${ProgramFilesX86}}/ARM/Mali Developer Tools/Mali OpenGL ES Emulator*/include"
			)
	ELSEIF(ANDROID)
		FIND_PATH(SAURON_GLES_INCLUDE_DIR KHR/khrplatform.h
			PATHS
				"$ENV{ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}/usr/include"
				"$ENV{ANDROID_NDK}/sysroot/usr/include"
		)
	ELSEIF(IOS)
		FIND_PATH(SAURON_GLES_INCLUDE_DIR EAGL.h
			PATHS
				/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/OpenGLES.framework/Headers
		)
	ELSEIF(APPLE)
		FIND_PATH(SAURON_GLES_INCLUDE_DIR KHR/khrplatform.h
			PATHS
				/Users/Shared/Imagination/PowerVR_Graphics/PowerVR_SDK/SDK_3.4/Builds/Include
		)
	ELSEIF(UNIX)
		FIND_PATH(SAURON_GLES_INCLUDE_DIR KHR/khrplatform.h
			PATHS
				/usr/local/include
				/usr/include
				/usr/local/X11R6/include
				/usr/X11R6/include
		)
	ENDIF()
ENDIF()

IF(SAURON_GLES_INCLUDE_DIR)
	SET(SAURON_GLES_FOUND TRUE)
	MESSAGE(STATUS "Found GLES include: ${SAURON_GLES_INCLUDE_DIR}")

	FIND_LIBRARY(SAURON_GLES_LIBRARY NAMES GLESv2)
	MESSAGE(STATUS "Found GLES library: ${SAURON_GLES_LIBRARY}")
	FIND_LIBRARY(SAURON_EGL_LIBRARY NAMES EGL)
	MESSAGE(STATUS "Found EGL library: ${SAURON_EGL_LIBRARY}")
ELSE()
	MESSAGE(STATUS "Could NOT find GLES.")
ENDIF()
