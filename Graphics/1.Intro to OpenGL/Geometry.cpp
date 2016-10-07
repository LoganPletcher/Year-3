#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Geometry.h"
#include "gl_core_4_4.h"
#include <iostream>
#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
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
using glm::vec2;
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
	int rows = 64, cols = 64;
	Vertex* vertices = new Vertex[rows * cols];
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			// offset position so that the terrain is centered
			vertices[r * cols + c].position = vec4((c - cols) * 0.5f, 0, (r - rows) * 0.5f, 1);
			/*if ((r * cols + c)%2==0)
			{
				vertices[r * cols + c].position.y = 1;
			}*/
			// setting up UVs
			vertices[r * cols + c].texCoord = vec2(c * (1.f / cols), r * (1.f / rows));
		}
	}
	// keep track of number of indices for rendering

	m_indexCount = (rows - 1) * (cols - 1) * 6;
	unsigned int* indices = new unsigned int[m_indexCount];
	unsigned int index = 0;
	for (int r = 0; r < (rows - 1); ++r) {
		for (int c = 0; c < (cols - 1); ++c) {
			// triangle 1
			indices[index++] = r * cols + c;
			indices[index++] = (r + 1) * cols + c;
			indices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			indices[index++] = r * cols + c;
			indices[index++] = (r + 1) * cols + (c + 1);
			indices[index++] = r * cols + (c + 1);
		}
	}

	int dims = 64;
	float *perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * 3;
	int octaves = 6;
	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 64; ++y)
		{
			float amplitude = 1.f;
			float persistence = 0.3f;
			perlin_data[y * dims + x] = 0;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample =
					glm::perlin(vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}
	
	for (int i = 0; i < cols * rows; i++)
	{

		vertices[i].position.y = perlin_data[i];
	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* cols * rows, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexCount, indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((void*)0));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool Geometry::createShader()
{
	txtfilereader sourceData = txtfilereader();

	std::string stringvsSource = sourceData.loadFile("vsSourcePerlin.txt");
	const char* vsSource = stringvsSource.c_str();

	std::string stringfsSource = sourceData.loadFile("fsSourcePerlin.txt");
	const char* fsSource = stringfsSource.c_str();

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

bool Geometry::TextureLoad3D(char* filename, char* filename2)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(filename2, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture[0]);
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return true;
}

bool Geometry::TextureLoadA(char* filename, int i)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture[i]);
	glBindTexture(GL_TEXTURE_2D, m_texture[i]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);


	stbi_image_free(data);

	return true;
}

bool Geometry::TextureLoadB(char* filename, int i)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture[i]);
	glBindTexture(GL_TEXTURE_2D, m_texture[i]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	stbi_image_free(data);

	return true;
}

bool Geometry::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1080, 720);

	// setup camera// setup camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	generateGrid();
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
	int matUniform = glGetUniformLocation(m_programID, "view_proj");

	// send the matrix
	glUniformMatrix4fv(matUniform, 1, GL_FALSE, &(m_camera->getProjectionView()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	matUniform = glGetUniformLocation(m_programID, "perlin_texture");
	glUniform1i(matUniform, 0);

	// draw quad
	glBindVertexArray(m_VAO);
	glPointSize(5.0f);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

}

void Geometry::inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	Geometry* myThis = (Geometry*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE)
		if (action == GLFW_PRESS)
			myThis->m_direction = true;
		else if (action == GLFW_RELEASE)
			myThis->m_direction = false;
}