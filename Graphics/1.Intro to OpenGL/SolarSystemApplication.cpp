#include "SolarSystemApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
//#include "Gizmos.h"

#include<iostream>
using glm::vec3;
using glm::vec4;
using glm::mat4;

SolarSystemApplication::SolarSystemApplication()
	: m_camera(nullptr),
	m_direction(false) {

}

SolarSystemApplication::~SolarSystemApplication() {

}

bool SolarSystemApplication::generateGrid()
{
	Vertex Vertices[4];
	unsigned int Indices[4] = { 0,2,1,3 };

	Vertices[0].position = vec4(-5, 0, -5, 1);
	Vertices[1].position = vec4(5, 0, -5, 1);
	Vertices[2].position = vec4(-5, 0, 5, 1);
	Vertices[3].position = vec4(5, 0, 5, 1);

	Vertices[0].color = vec4(1, 0, 0, 1);
	Vertices[1].color = vec4(0, 1, 0, 1);
	Vertices[2].color = vec4(0, 0, 1, 1);
	Vertices[3].color = vec4(1, 1, 1, 1);
	// create and bind buffers to a vertex array object


	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);


	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), Vertices, GL_STATIC_DRAW);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), Indices, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));



	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return true;
}

bool SolarSystemApplication::createShader()
{
	// create shader
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							out vec4 vColour; \
							uniform mat4 ProjectionViewWorldMatrix; \
							void main() { vColour = Colour; \
							gl_Position = ProjectionViewWorldMatrix * Position; }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 FragColor; \
							void main() { FragColor = vColour; }";


	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	// check that it compiled and linked correctly
	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength + 1];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// we don't need to keep the individual shaders around
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return true;

}

bool SolarSystemApplication::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1080, 720);

	// start the gizmo system that can draw basic immediate-mode shapes
	//Gizmos::create();

	// setup camera// setup camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));



	generateGrid();
	createShader();
	// set input callback
	setInputCallback(inputCallback);

	return true;
}

void SolarSystemApplication::shutdown() {

	// cleanup camera and gizmos
	delete m_camera;
	//Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool SolarSystemApplication::update(float deltaTime) {

	// close the application if the window closes or we press escape
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	//// update the camera's movement
	m_camera->update(deltaTime);

	//// clear the gizmos out for this frame
	////Gizmos::clear();

	//vec4 white(1, 1, 1, 0);
	//vec4 black(0, 0, 0, 1);

	//// for now let's add a grid to the gizmos
	//for (int i = 0; i < 21; ++i) {
	//	//Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10),
	//		//i == 10 ? white : black);
	//	//Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i),
	//		//i == 10 ? white : black);
	//}

	////Gizmos::addTransform(mat4(1));

	//float time = (float)glfwGetTime();

	//// simple solar system
	////
	////// sun spins (reverses if direction says to)
	////mat4 sun = glm::rotate(time * 0.1f * (m_direction ? -1 : 1), vec3(0, 1, 0));

	////// earth spins around sun
	////mat4 earth = sun * glm::translate(vec3(8,0,0)) * glm::rotate(time, vec3(0,1,0));

	////// moon spins around earth but always facing the earth the same way
	////mat4 moon = earth * glm::rotate(time * 1.5f, vec3(0, 1, 0)) * glm::translate(vec3(1.5f, 0, 0));
	//
	//// add simple spheres for each
	////Gizmos::addSphere(vec3(sun[3]), 2, 16, 16, vec4(1, 1, 0, 1), &sun);
	////Gizmos::addSphere(vec3(earth[3]), 0.5f, 16, 16, vec4(0, 0, 1, 1), &earth);
	////Gizmos::addSphere(vec3(moon[3]), 0.125f, 16, 16, vec4(0.6f, 0.6f, 0.6f, 1), &moon);

	//// lines and transforms show how the matrices are relative
	////Gizmos::addLine(vec3(sun[3]), vec3(earth[3]), vec4(1, 0, 0, 1));
	////Gizmos::addLine(vec3(earth[3]), vec3(moon[3]), vec4(1, 0, 0, 1));
	////Gizmos::addTransform(sun, 3);
	////Gizmos::addTransform(earth, 1);
	////Gizmos::addTransform(moon, 0.5f);

	//// return true, else the application closes

	//

	return true;
}

void SolarSystemApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind shader
	glUseProgram(m_programID);

	// where to send the matrix
	int matUniform = glGetUniformLocation(m_programID, "ProjectionViewWorldMatrix");

	// send the matrix
	glUniformMatrix4fv(matUniform, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionView()));

	// draw quad
	glBindVertexArray(m_VAO);

	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);

}

void SolarSystemApplication::inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	SolarSystemApplication* myThis = (SolarSystemApplication*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE)
		if (action == GLFW_PRESS)
			myThis->m_direction = true;
		else if (action == GLFW_RELEASE)
			myThis->m_direction = false;
}