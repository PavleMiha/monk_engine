/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"
#include "bgfx/bgfx.h"
#include "bx/file.h"
#include "bx/bounds.h"

#include "vertex.h"

#include <string>
#include <vector>

struct Group
{
	Group();
	void reset();

	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;
	uint16_t m_numVertices;
	uint8_t* m_vertices;
	uint32_t m_numIndices;
	uint16_t* m_indices;
	bx::Sphere m_sphere;
	bx::Aabb   m_aabb;
	bx::Obb    m_obb;
	//PrimitiveArray m_prims;
};
//typedef stl::vector<Group> GroupArray;

struct Mesh
{
	void load(const std::string& filepath);
	void submit(bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state = BGFX_STATE_MASK) const;

	std::vector<PosNormalTexVertex> vertices;
	std::vector<u16> indices;

	std::vector<Group> m_groups;
	bgfx::VertexLayout m_layout;
};


