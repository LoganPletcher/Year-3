#include "BaseApplication.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

bool BaseApplication::createWindow(const char* title, int width, int height) {

	if (glfwInit() == GL_FALSE)
		return false;

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_window == nullptr) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h){ glViewport(0, 0, w, h); });

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	glClearColor(0.25f, 0.25f, 0.25f, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	return true;
}

void BaseApplication::destroyWindow() {

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void BaseApplication::run() {

	double prevTime = glfwGetTime();
	double currTime = 0;

	while (currTime = glfwGetTime(),
		update((float)(currTime - prevTime))) {

		
		draw();
		glfwSwapBuffers(m_window);
		glfwPollEvents();

		prevTime = currTime;
	}
}

void BaseApplication::setInputCallback(void(*func)(GLFWwindow* window, int key, int scancode, int action, int mods)) {
	glfwSetWindowUserPointer(m_window, this);
	glfwSetKeyCallback(m_window, func);
}