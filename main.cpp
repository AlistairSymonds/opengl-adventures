#include <iostream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "GLow.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main() {
	glfwSetErrorCallback(&glfwError);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	int window_h = 1080;
	int window_w = 1920;

	//create a window
	GLFWwindow* window = glfwCreateWindow(window_w, window_h, "OGL Adventure", NULL, NULL);
	if (window == NULL) {
		std::cout << "No window :(" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "No GLAD :(" << std::endl;
		return 1;
	}

	//set a default size, then also make resizing easy
	glViewport(0, 0, window_w, window_h);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	GLow glow;
	glow.compileShaders();
	glow.setRenderProgram();
	//go go gadget render loop
	while (!glfwWindowShouldClose(window))
	{	
		//read command
		processInput(window);
		
		//render new state
		glow.render();

		//display rendered output
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}