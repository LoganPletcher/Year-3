#pragma once

#include "BaseApplication.h"

// forward declared to reduce dependencies
class Camera;
class Mesh;
class Shader;

class ApplyTextureOBJApplication : public BaseApplication {
public:

	ApplyTextureOBJApplication();
	virtual ~ApplyTextureOBJApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	Camera*		m_camera;
	Mesh*		m_mesh;
	Shader*		m_shader;
};