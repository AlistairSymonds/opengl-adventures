#pragma once
// main rendering class, can be interperated one of two ways:
// glowing, since it produces beautiful output
// GL OW, because it hurts my brain
//  (im sure i'm not the frst person to use this name)




#include <filesystem>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define glCheckError() glCheckError_(__FILE__, __LINE__) 

class GLow
{
public:
	GLow();
	int render();
	int compileShaders();
	int setRenderProgram();

	GLenum glCheckError_(const char* file, int line)
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			std::cout << error << " | " << file << " (" << line << ")" << std::endl;
		}
		return errorCode;
	}

private:
	std::filesystem::path shader_dir;


	typedef enum shaders_t {
		default_vtx = 1,
		default_frag = 2,
		tex_frag = 3,
		ERROR = 256
	} shaders_t;
	
	shaders_t file_name_to_shader_id(std::filesystem::path p);

	GLenum shader_id_to_gl_type(shaders_t s);
	std::unordered_map<shaders_t, GLuint> shaders;

	uint32_t shader_prog;
};
