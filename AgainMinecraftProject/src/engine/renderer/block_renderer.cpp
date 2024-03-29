#include <stdint.h>
#include <glad/glad.h>

#include "../ray/ray.h"

#include "block_renderer.h"

using namespace Engine::Renderer;

static IBuffer	g_cubeOutlineBuff;
static Buffer	g_cubeBuff;
static IBuffer	g_pOutlineBuff;
static Buffer	g_rayBuff;
static Buffer	g_quadBuffer;

// This might not be needed anymore
constexpr float g_cubeOutlineVerts[] = {
	0, 0, 0, // -x -y -z (0)
	0, 1, 0, // -x +y -z (1)
	1, 1, 0, // +x +y -z (2)
	1, 0, 0, // +x -y -z (3)
	0, 0, 1, // -x -y +z (4)
	0, 1, 1, // -x +y +z (5)
	1, 1, 1, // +x +y +z (6)
	1, 0, 1, // +x -y +z (7)
};

constexpr float g_cubeVerts[] = {
	// front
	0, 0, 0,
	1, 0, 0,
	0, 1, 0,
	1, 0, 0,
	0, 1, 0,
	1, 1, 0,

	// back
	0, 0, 1,
	1, 0, 1,
	0, 1, 1,
	1, 0, 1,
	0, 1, 1,
	1, 1, 1,

	// left
	0, 0, 0,
	0, 0, 1,
	0, 1, 0,
	0, 0, 1,
	0, 1, 0,
	0, 1, 1,

	// right
	1, 0, 0,
	1, 0, 1,
	1, 1, 0,
	1, 0, 1,
	1, 1, 0,
	1, 1, 1,

	// top
	0, 1, 0,
	1, 1, 0,
	1, 1, 1,
	1, 1, 1,
	0, 1, 1,
	0, 1, 0,
	
	// bottom
	0, 0, 0,
	1, 0, 0,
	1, 0, 1,
	1, 0, 1,
	0, 0, 1,
	0, 0, 0
};

constexpr float g_playerOutlineVerts[] = {
	0.0f, 0.0f, 0.0f, // -x -y -z (0)
	0.0f, 2.0f, 0.0f, // -x +y -z (1)
	0.6f, 2.0f, 0.0f, // +x +y -z (2)
	0.6f, 0.0f, 0.0f, // +x -y -z (3)
	0.0f, 0.0f, 0.6f, // -x -y +z (4)
	0.0f, 2.0f, 0.6f, // -x +y +z (5)
	0.6f, 2.0f, 0.6f, // +x +y +z (6)
	0.6f, 0.0f, 0.6f, // +x -y +z (7)
};

constexpr uint32_t g_cubeOutlineInds[] = {
	0, 1,	1, 2,	2, 3,	3, 0,	// front
	4, 5,	5, 6,	6, 7,	7, 4,	// back
	0, 4,	1, 5,					// left
	3, 7,	2, 6					// right
};

constexpr float g_quadVertices[] = {
	// positions    // texture Coords
	-1, -1, 0,		0, 0,
	 1, -1, 0,		1, 0,
	-1,  1, 0,		0, 1,

	 1,  1, 0,		1, 1,
	-1,  1, 0,		0, 1,
	 1, -1, 0,		1, 0
};

constexpr float g_lineWidth = 3.0f;

void Engine::Renderer::loadCubeData()
{
	// Outline cube
	glGenVertexArrays(1, &g_cubeOutlineBuff.VAO);
	glGenBuffers(1, &g_cubeOutlineBuff.VBO);
	glGenBuffers(1, &g_cubeOutlineBuff.IBO);

	glBindVertexArray(g_cubeOutlineBuff.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_cubeOutlineBuff.VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_cubeOutlineBuff.IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeOutlineVerts), g_cubeOutlineVerts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_cubeOutlineInds), g_cubeOutlineInds, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glLineWidth(g_lineWidth);
	
	// Cube
	glGenVertexArrays(1, &g_cubeBuff.VAO);
	glGenBuffers(1, &g_cubeBuff.VBO);

	glBindVertexArray(g_cubeBuff.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_cubeBuff.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeVerts), g_cubeVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Engine::Renderer::loadPlayerOutlineData()
{
	glGenVertexArrays(1, &g_pOutlineBuff.VAO);
	glGenBuffers(1, &g_pOutlineBuff.VBO);
	glGenBuffers(1, &g_pOutlineBuff.IBO);

	glBindVertexArray(g_pOutlineBuff.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_pOutlineBuff.VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_pOutlineBuff.IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(g_playerOutlineVerts), g_playerOutlineVerts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_cubeOutlineInds), g_cubeOutlineInds, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glLineWidth(g_lineWidth);
}

void Engine::Renderer::loadRayData(const Ray& ray)
{
	const float vertices[] = {
		ray.start.x, ray.start.y, ray.start.z,
		ray.end.x, ray.end.y, ray.end.z
	};

	glGenVertexArrays(1, &g_rayBuff.VAO);
	glGenBuffers(1, &g_rayBuff.VBO);

	glBindVertexArray(g_rayBuff.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, g_rayBuff.VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glLineWidth(g_lineWidth);
}

void Engine::Renderer::loadQuadData()
{
	glGenVertexArrays(1, &g_quadBuffer.VAO);
	glGenBuffers(1, &g_quadBuffer.VBO);

	glBindVertexArray(g_quadBuffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_quadBuffer.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quadVertices), &g_quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Engine::Renderer::render(const Type type)
{
	switch (type)
	{
	case Type::CUBE:
		glBindVertexArray(g_cubeBuff.VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		break;

	case Type::CUBE_LINES:
		glBindVertexArray(g_cubeOutlineBuff.VAO);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
		break;

	case Type::QUAD:
		glBindVertexArray(g_quadBuffer.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		break;
	
	case Type::RAY:
		glBindVertexArray(g_rayBuff.VAO);
		glDrawArrays(GL_LINES, 0, 2);
		break;

	case Type::PLAYER:
		glBindVertexArray(g_pOutlineBuff.VAO);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
		break;
	}
}
