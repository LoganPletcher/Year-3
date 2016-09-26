#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Geometry.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "TXTFileReader.h"
//	                                   _._
//	                              _.-="_-         _
//	                         _.-="   _-          | ||"""""""---._______     __..
//	             ___.===""""-.______-,,,,,,,,,,,,`-''----" """""       """""  __'
//	      __.--""     __        ,'                   o \           __        [__|
//	 __-""=======.--""  ""--.=================================.--""  ""--.=======:
//	]       [w] : /        \ : |========================|    : /        \ :  [w] :
//	V___________:|          |: |========================|    :|          |:   _-"
//	 V__________: \        / :_|=======================/_____: \        / :__-"
//	 -----------'  ""____""  `-------------------------------'  ""____""
using glm::vec3;
using glm::vec4;
using glm::mat4;

Geometry::Geometry()
{

}

Geometry::~Geometry()
{

}

bool Geometry::generateGrid()
{
	//Vertex Vertices[4];
	//unsigned int Indices[4] = { 0,2,1,3 };

	//Vertices[0].position = vec4(-5, 0, -5, 1);
	//Vertices[1].position = vec4(5, 0, -5, 1);
	//Vertices[2].position = vec4(-5, 0, 5, 1);
	//Vertices[3].position = vec4(5, 0, 5, 1);

	//Vertices[0].color = vec4(1, 0, 0, 1);
	//Vertices[1].color = vec4(0, 1, 0, 1);
	//Vertices[2].color = vec4(0, 0, 1, 1);
	//Vertices[3].color = vec4(1, 1, 1, 1);
	// create and bind buffers to a vertex array object

	float vertexData[] = {
		-5, 0, 5, 1, 0, 1,
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
		-5, 0, -5, 1, 0, 0,};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0)+16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool Geometry::createShader()
{
	txtfilereader sourceData = txtfilereader();
	// create shader
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec2 TexCoord; \
							out vec2 vTexCoord; \
							uniform mat4 ProjectionView; \
							void main() { \
							vTexCoord = TexCoord; \
							gl_Position= ProjectionView * Position;\}";
	const char* fsSource = "#version 410\n \
							in vec2 vTexCoord; \
							out vec4 FragColor; \
							uniform sampler2D diffuse; \
							void main() { \
							FragColor = texture(diffuse,vTexCoord);\}";

	//std::string stringvsSource = sourceData.loadFile("vsSource.txt");
	//const char* vsSource = stringvsSource.c_str();
	//std::string stringfsSource = sourceData.loadFile("fsSource.txt");
	//const char* fsSource = stringfsSource.c_str();

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

std::vector<glm::vec4> Geometry::makeSphere(std::vector<glm::vec4> start, int meridians, int points)
{
	std::vector<glm::vec4> vertices;
	for (int i = 0; i < start.size(); i++)
		vertices.push_back(start[i]);
	for (int i = 1; i <= meridians; i++)
	{
		float phi = 2.f * glm::pi<float>()  * (float)i / (float)meridians;
		for (int j = 0; j < start.size(); j++)
		{
			float newX = vertices[j].x;//+ 0 + 0
			float newY =	/*0 + */		(vertices[j].y * cos(phi)) - (vertices[j].z * sin(phi));
			float newZ =	/*0 + */		(vertices[j].y * sin(phi)) + (vertices[j].z * cos(phi));

			vertices.push_back(vec4(newX, newY, newZ, 1.f));
		}
	}
	return vertices;
}

std::vector<glm::vec4> Geometry::makeHalfCircle(int points, float radius)
{
	std::vector<glm::vec4> v;
	float theta;
	for (int i = 0; i < points; i++)
	{
		theta = 3.14159274f * (float)i / (float)(points - 1);
		float newX = radius * cosf(theta);
		float newY = radius * sinf(theta);
		v.push_back(vec4(newX, newY, 0.f, 1));
	}
	return v;
}

std::vector<unsigned int> Geometry::GenerateIndices(int meridians, int points)
{

	//j=np-1
	//      
	//2   5   8   11  14  17
	//1   4   7   10  13  16
	//0   3   6   9   12  15      
	//  
	for (unsigned int i = 0; i < meridians; i++) //nm = 4
	{
		unsigned int start = i * points;
		for (int j = 0; j < points; j++) //np = 3
		{
			unsigned int botR = (start + points + j);
			unsigned int botL = (start + j);
			indices.push_back(botL);
			indices.push_back(botR);
		}
		indices.push_back(0xFFFF);
	} //we copied the origin whenever we rotated around nm + 1 times so we dont need to get the end again
	return indices;
}

bool Geometry::generateSphere()
{
	int np = 800;
	int nm = 80;
	int radius = 5.f;
	std::vector<vec4> halfCirc = makeHalfCircle(np, radius);
	std::vector<vec4> fullCirc = makeSphere(halfCirc, nm, np);
	indices = GenerateIndices(nm, np);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);


	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, fullCirc.size() * sizeof(vec4), fullCirc.data(), GL_STATIC_DRAW);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(glm::vec2));



	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool Geometry::TextureLoad(char* filename)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	stbi_image_free(data);	return true;
}

bool Geometry::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1080, 720);

	// start the gizmo system that can draw basic immediate-mode shapes
	//Gizmos::create();

	// setup camera// setup camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	TextureLoad("./dep/stb-master/data/textures/lightening.png");

	//int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	//unsigned char* data = stbi_load("./dep/stb-master/data/textures/crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	//glGenTextures(1, &m_texture);
	//glBindTexture(GL_TEXTURE_2D, m_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//stbi_image_free(data);	//return 1;

	generateGrid();
	//generateSphere();
	createShader();
	// set input callback
	setInputCallback(inputCallback);

	return true;
}

void Geometry::shutdown() {

	// cleanup camera and gizmos
	delete m_camera;
	//Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool Geometry::update(float deltaTime) {

	// close the application if the window closes or we press escape
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	//// update the camera's movement
	m_camera->update(deltaTime);

	return true;
}

void Geometry::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind shader
	glUseProgram(m_programID);

	// where to send the matrix
	int matUniform = glGetUniformLocation(m_programID, "ProjectionView");

	// send the matrix
	glUniformMatrix4fv(matUniform, 1, GL_FALSE, &(m_camera->getProjectionView()[0][0]));

	// set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	matUniform = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(matUniform, 0);
	// draw quad
	glBindVertexArray(m_VAO);
	//glEnable(GL_PRIMITIVE_RESTART);
	//glPrimitiveRestartIndex(0xFFFF);
	glPointSize(5.f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

void Geometry::inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	Geometry* myThis = (Geometry*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE)
		if (action == GLFW_PRESS)
			myThis->m_direction = true;
		else if (action == GLFW_RELEASE)
			myThis->m_direction = false;
}