/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"
#include "bgfx/bgfx.h"
#include "bx/file.h"
#include "bx/bounds.h"

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
	void load(bx::ReaderSeekerI* _reader, bool _ramcopy);

	bgfx::VertexLayout m_layout;

};


