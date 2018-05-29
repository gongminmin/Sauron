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

#include <Sauron/GraphicsSystem.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <boost/assert.hpp>

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>

#include <Sauron/ErrorHandling.hpp>

namespace
{
	char const * DebugSourceString(GLenum value)
	{
		char const * ret;
		switch (value)
		{
		case GL_DEBUG_SOURCE_API_KHR:
			ret = "GL";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_KHR:
			ret = "shader compiler";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR:
			ret = "window system";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_KHR:
			ret = "3rd party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION_KHR:
			ret = "application";
			break;
		case GL_DEBUG_SOURCE_OTHER_KHR:
			ret = "other";
			break;

		default:
			SAURON_UNREACHABLE("Invalid debug source");
		}

		return ret;
	}

	char const * DebugTypeString(GLenum value)
	{
		char const * ret;
		switch (value)
		{
		case GL_DEBUG_TYPE_ERROR_KHR:
			ret = "error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR:
			ret = "deprecated behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR:
			ret = "undefined behavior";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE_KHR:
			ret = "performance";
			break;
		case GL_DEBUG_TYPE_PORTABILITY_KHR:
			ret = "portability";
			break;
		case GL_DEBUG_TYPE_MARKER_KHR:
			ret = "marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP_KHR:
			ret = "push group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP_KHR:
			ret = "pop group";
			break;
		case GL_DEBUG_TYPE_OTHER_KHR:
			ret = "other";
			break;

		default:
			SAURON_UNREACHABLE("Invalid debug type");
		}

		return ret;
	}

	char const * DebugSeverityString(GLenum value)
	{
		char const * ret;
		switch (value)
		{
		case GL_DEBUG_SEVERITY_HIGH_KHR:
			ret = "high";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM_KHR:
			ret = "medium";
			break;
		case GL_DEBUG_SEVERITY_LOW_KHR:
			ret = "low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
			ret = "notification";
			break;

		default:
			SAURON_UNREACHABLE("Invalid debug severity");
		}

		return ret;
	}

	void KHRONOS_APIENTRY DebugOutputProc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		GLchar const * message, void const * user_param)
	{
		SAURON_UNUSED(length);
		SAURON_UNUSED(user_param);

		std::string dbg = std::string("OpenGL debug output: source: ") + DebugSourceString(source) + "; "
			+ "type: " + DebugTypeString(type) + "; "
			+ "id: " + std::to_string(id) + "; "
			+ "severity: " + DebugSeverityString(severity) + "; "
			+ "message: " + message;
		if (GL_DEBUG_TYPE_ERROR_KHR == type)
		{
			std::cerr << dbg << std::endl;
		}
		else
		{
			std::cout << dbg << std::endl;
		}
	}

	void PrintError(GLuint shader)
	{
		GLint compiled = false;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			std::cerr << "Error when compiling GLSL:" << std::endl;

			GLint len = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
			if (len > 0)
			{
				std::vector<char> info(len + 1, 0);
				glGetShaderInfoLog(shader, len, &len, &info[0]);
				std::cerr << info.data() << std::endl;
			}
		}
	}

	GLuint CreateGLSLProgram(char const * vs_str, char const * fs_str)
	{
		GLuint program = glCreateProgram();

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vs_str, nullptr);
		glCompileShader(vs);
		PrintError(vs);

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fs_str, nullptr);
		glCompileShader(fs);
		PrintError(fs);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
}

namespace Sauron
{
	GraphicsSystem::GraphicsSystem(void* wnd)
	{
		display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		EGLint visual_attr[] =
		{
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
			EGL_NONE
		};

		EGLint egl_major_ver, egl_minor_ver;
		eglInitialize(display_, &egl_major_ver, &egl_minor_ver);

		EGLint num_cfgs;
		eglChooseConfig(display_, &visual_attr[0], &cfg_, 1, &num_cfgs);

		surf_ = eglCreateWindowSurface(display_, cfg_, static_cast<EGLNativeWindowType>(wnd), nullptr);

		context_ = nullptr;
		EGLint ctx_attr[] =
		{
			EGL_CONTEXT_MAJOR_VERSION, 3,
			EGL_NONE
		};
		context_ = eglCreateContext(display_, cfg_, EGL_NO_CONTEXT, ctx_attr);

		eglMakeCurrent(display_, surf_, surf_, context_);

		eglSwapInterval(display_, 0);

		// TODO: Detects GL_KHR_debug extension
		glEnable(GL_DEBUG_OUTPUT_KHR);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
		glDebugMessageCallbackKHR(&DebugOutputProc, nullptr);
		glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH_KHR, 0, nullptr, GL_TRUE);
		glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM_KHR, 0, nullptr, GL_TRUE);
		glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW_KHR, 0, nullptr, GL_FALSE);
		glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION_KHR, 0, nullptr, GL_FALSE);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		this->InitShaders();
	}

	GraphicsSystem::~GraphicsSystem()
	{
		this->DeinitShaders();

		if (display_ != nullptr)
		{
			eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglDestroyContext(display_, context_);
			eglTerminate(display_);

			display_ = nullptr;
		}
	}

	void GraphicsSystem::InitShaders()
	{
		std::cout << "Initializing basic GL shaders... " << std::endl;

		{
			// Vertex filled with plain color

			char const * vs_str =
				"precision highp float;\n"
				"\n"
				"attribute vec3 position;\n"
				"\n"
				"uniform mat4 projection_matrix;\n"
				"\n"
				"void main()\n"
				"{\n"
				"	gl_Position = projection_matrix * vec4(position, 1.0);\n"
				"}\n";
			char const * fs_str =
				"precision highp float;\n"
				"\n"
				"uniform vec4 color;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = color;\n"
				"}\n";

			basic_shader_program_ = CreateGLSLProgram(vs_str, fs_str);

			basic_shader_vars_.projection_matrix = glGetUniformLocation(basic_shader_program_, "projection_matrix");
			BOOST_ASSERT(basic_shader_vars_.projection_matrix != -1);
			basic_shader_vars_.color = glGetUniformLocation(basic_shader_program_, "color");
			BOOST_ASSERT(basic_shader_vars_.color != -1);
			basic_shader_vars_.position = glGetAttribLocation(basic_shader_program_, "position");
			BOOST_ASSERT(basic_shader_vars_.position != -1);
		}
		{
			// Vertex filled with interpolated color

			char const * vs_str =
				"precision highp float;\n"
				"\n"
				"attribute vec3 position;\n"
				"attribute vec3 color;\n"
				"\n"
				"uniform mat4 projection_matrix;\n"
				"\n"
				"varying vec4 v_frag_color;\n"
				"\n"
				"void main()\n"
				"{\n"
				"	gl_Position = projection_matrix * vec4(position, 1.0);\n"
				"	v_frag_color = vec4(color, 1.0);\n"
				"}\n";
			char const * fs_str =
				"precision highp float;\n"
				"\n"
				"varying vec4 v_frag_color;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = v_frag_color;\n"
				"}\n";

			color_shader_program_ = CreateGLSLProgram(vs_str, fs_str);

			color_shader_vars_.projection_matrix = glGetUniformLocation(color_shader_program_, "projection_matrix");
			BOOST_ASSERT(color_shader_vars_.projection_matrix != -1);
			color_shader_vars_.color = glGetAttribLocation(color_shader_program_, "color");
			BOOST_ASSERT(color_shader_vars_.projection_matrix != -1);
			color_shader_vars_.position = glGetAttribLocation(color_shader_program_, "position");
			BOOST_ASSERT(color_shader_vars_.projection_matrix != -1);
		}
		{
			// Texture shader program

			char const * vs_str =
				"precision highp float;\n"
				"\n"
				"attribute vec3 position;\n"
				"attribute vec2 tex_coord;\n"
				"\n"
				"uniform mat4 projection_matrix;\n"
				"\n"
				"varying vec2 v_tex_coord;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    gl_Position = projection_matrix * vec4(position, 1.0);\n"
				"    v_tex_coord = tex_coord;\n"
				"}\n";
			char const * fs_str =
				"precision highp float;\n"
				"\n"
				"varying vec2 v_tex_coord;\n"
				"\n"
				"uniform sampler2D tex;\n"
				"uniform vec4 tex_color;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = texture2D(tex, v_tex_coord) * tex_color;\n"
				"}\n";

			textures_shader_program_ = CreateGLSLProgram(vs_str, fs_str);

			textures_shader_vars_.projection_matrix = glGetUniformLocation(textures_shader_program_, "projection_matrix");
			BOOST_ASSERT(textures_shader_vars_.projection_matrix != -1);
			textures_shader_vars_.tex_coord = glGetAttribLocation(textures_shader_program_, "tex_coord");
			BOOST_ASSERT(textures_shader_vars_.tex_coord != -1);
			textures_shader_vars_.position = glGetAttribLocation(textures_shader_program_, "position");
			BOOST_ASSERT(textures_shader_vars_.position != -1);
			textures_shader_vars_.tex_color = glGetUniformLocation(textures_shader_program_, "tex_color");
			BOOST_ASSERT(textures_shader_vars_.tex_color != -1);
			textures_shader_vars_.texture = glGetUniformLocation(textures_shader_program_, "tex");
			BOOST_ASSERT(textures_shader_vars_.texture != -1);
		}
		{
			// Texture shader program + interpolated color per vertex

			char const * vs_str =
				"precision highp float;\n"
				"\n"
				"attribute vec3 position;\n"
				"attribute vec2 tex_coord;\n"
				"attribute vec3 color;\n"
				"\n"
				"uniform mat4 projection_matrix;\n"
				"\n"
				"varying vec2 v_tex_coord;\n"
				"varying vec4 v_color;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    gl_Position = projection_matrix * vec4(position, 1.0);\n"
				"    v_tex_coord = tex_coord;\n"
				"    v_color = vec4(color, 1.0);\n"
				"}\n";
			char const * fs_str =
				"precision highp float;\n"
				"\n"
				"varying vec2 v_tex_coord;\n"
				"varying vec4 v_color;\n"
				"\n"
				"uniform sampler2D tex;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = texture2D(tex, v_tex_coord) * v_color * 10.0;\n"
				"}\n";

			textures_color_shader_program_ = CreateGLSLProgram(vs_str, fs_str);

			textures_color_shader_vars_.projection_matrix = glGetUniformLocation(textures_color_shader_program_, "projection_matrix");
			BOOST_ASSERT(textures_color_shader_vars_.projection_matrix != -1);
			textures_color_shader_vars_.tex_coord = glGetAttribLocation(textures_color_shader_program_, "tex_coord");
			BOOST_ASSERT(textures_color_shader_vars_.tex_coord != -1);
			textures_color_shader_vars_.position = glGetAttribLocation(textures_color_shader_program_, "position");
			BOOST_ASSERT(textures_color_shader_vars_.position != -1);
			textures_color_shader_vars_.color = glGetAttribLocation(textures_color_shader_program_, "color");
			BOOST_ASSERT(textures_color_shader_vars_.color != -1);
			textures_color_shader_vars_.texture = glGetUniformLocation(textures_color_shader_program_, "tex");
			BOOST_ASSERT(textures_color_shader_vars_.texture != -1);
		}
	}

	void GraphicsSystem::DeinitShaders()
	{
		glDeleteProgram(basic_shader_program_);
		glDeleteProgram(color_shader_program_);
		glDeleteProgram(textures_shader_program_);
		glDeleteProgram(textures_color_shader_program_);
	}

	void GraphicsSystem::PreDraw()
	{
		float depth = 1;
		glClearBufferfv(GL_DEPTH, 0, &depth);
	}

	void GraphicsSystem::PostDraw()
	{
		eglSwapBuffers(display_, surf_);
	}
}
