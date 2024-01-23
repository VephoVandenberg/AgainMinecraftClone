#pragma once

#include <glm/glm.hpp>
#include <stdint.h>
#include <queue>
#include <vector>

#include "../../engine/renderer/mesh.h"

#include "block.h"

namespace Engine
{
	struct Ray;
	struct Shader;
}

namespace GameModule
{
	struct Block;

	struct Chunk
	{
		glm::ivec3	front;
		glm::ivec3	back;
		glm::ivec3	left;
		glm::ivec3	right;

		bool					updated = false;
		glm::vec3				pos;
		std::vector<Block>		blocks = {};
		Engine::Renderer::Mesh	solidMesh;
		Engine::Renderer::Mesh	transparentMesh;

		Engine::Renderer::MeshBuffer* solidBuffer = nullptr;
		Engine::Renderer::MeshBuffer* transBuffer = nullptr;
	};

	enum class RayType
	{
		IDLE,
		REMOVE,
		PLACE
	};

	Chunk	generateChunk(const glm::ivec3& pos);
	void	updateMesh(Chunk& chunk, Engine::Renderer::MeshBuffer& transBuffer, Engine::Renderer::MeshBuffer& solidBuffer);
	void	updateChunkNeighbourFace(Chunk& chunk1, Chunk& chunk2);

	void	setBlockFace(Chunk& chunk, const glm::vec3& pos, BlockType type, Face::FaceType face);
	void	removeBlockFace(Chunk& chunk, uint32_t id, Face::FaceType type);

	void	 loadChunkMesh(Chunk& chunk);
	void	 drawSolid(const Chunk& chunk);
	void	 drawTrans(const Chunk& chunk);
	uint32_t disableChunk(Chunk& chunk);
}
