#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>
#include <imgui.h>
#include <Windows.h>

using namespace glm;

struct Camera
{
	vec3 m_pos;
	vec3 m_look;
	vec3 m_up;
	vec3 m_left;
	mat4 view;

	float m_fov;
	float m_aspect;
	float m_near;
	float m_far;
	mat4 proj;
};

class MeshViewer
{
private:
	std::string m_name;
public:
	void init()
	{

	}
	void release()
	{

	}
	void beginRender()
	{

	}
	void drawMesh()
	{

	}
	void endRender()
	{

	}
};