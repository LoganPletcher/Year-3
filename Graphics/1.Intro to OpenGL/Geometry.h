#pragma once
#include "BaseApplication.h"


#include <glm/vec4.hpp>
#include <vector>

class Camera;

class Geometry : public BaseApplication
{
public:
	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 color;
	};
	Geometry();
	~Geometry();
	bool generateGrid();
	bool createShader();
	std::vector<glm::vec4> makeSphere(std::vector<glm::vec4>, int, int);
	std::vector<glm::vec4> makeHalfCircle(int, float);
	bool generateSphere();
	std::vector<unsigned int> GenerateIndices(int, int);
	bool TextureLoadA(char*);
	bool TextureLoadB(char*);
	bool TextureLoad3D(char*, char*);
	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

	static void inputCallback(GLFWwindow* window, int key, int scanline, int action, int mods);
	
	
private:
	std::vector<unsigned int> indices;

	Camera*	m_camera;
	// we can toggle the way the earth orbits the sun,
	// demonstating input callback
	bool m_direction;
	// our vertex and index buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_programID;
	unsigned int m_texture, m_normal;

};
