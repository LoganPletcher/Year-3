#pragma once
#include "BaseApplication.h"


#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <vector>

class Camera;

class Geometry : public BaseApplication
{
public:
	struct Vertex
	{
		glm::vec4 position;
		glm::vec2 texCoord;
		glm::vec4 color;
	};
	struct vertex {
		float x, y, z, w;
		float nx, ny, nz, nw;
		float tx, ty, tz, tw;
		float s, t;
	};
	Geometry();
	~Geometry();
	bool generateGrid();
	bool createShader();
	std::vector<glm::vec4> makeSphere(std::vector<glm::vec4>, int, int);
	std::vector<glm::vec4> makeHalfCircle(int, float);
	bool generateSphere();
	std::vector<unsigned int> GenerateIndices(int, int);
	bool TextureLoad3D(char*, char*);
	bool TextureLoadA(char*, int);
	bool TextureLoadB(char*, int);
	bool PerlinTexture(char*, int, float*);
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
	unsigned int m_texture[10], m_normal, m_perlin_texture;
	unsigned int m_indexCount;

};
