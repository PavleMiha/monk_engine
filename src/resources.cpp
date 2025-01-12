/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#include "resources.h"

#include <bx/file.h>
#include <bx/allocator.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "vertex.h"

bgfx::VertexLayout PosColorVertex::s_layout = bgfx::VertexLayout();
bgfx::VertexLayout PosNormalTexVertex::ms_layout = bgfx::VertexLayout();

extern bx::AllocatorI* getDefaultAllocator();

static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
	if (bx::open(_reader, _filePath))
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		const bgfx::Memory* mem = bgfx::alloc(size + 1);
		bx::read(_reader, mem->data, size, bx::ErrorAssert{});
		bx::close(_reader);
		mem->data[mem->size - 1] = '\0';
		return mem;
	}

	//DBG("Failed to load %s.", _filePath);
	return NULL;
}

void* load(bx::FileReaderI* _reader, bx::AllocatorI* _allocator, const char* _filePath, uint32_t* _size)
{
	if (bx::open(_reader, _filePath))
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		void* data = bx::alloc(_allocator, size);
		bx::read(_reader, data, size, bx::ErrorAssert{});
		bx::close(_reader);
		if (NULL != _size)
		{
			*_size = size;
		}
		return data;
	}
	else
	{
		//DBG("Failed to open: %s.", _filePath);
	}

	if (NULL != _size)
	{
		*_size = 0;
	}

	return NULL;
}


static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* _name)
{
	char filePath[512];

	const char* shaderPath = "???";

	switch (bgfx::getRendererType())
	{
	case bgfx::RendererType::Noop:
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
	case bgfx::RendererType::Agc:
	case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
	case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
	case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
	case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
	case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
	case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;

	case bgfx::RendererType::Count:
		BX_ASSERT(false, "You should not be here!");
		break;
	}

	bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
	bx::strCat(filePath, BX_COUNTOF(filePath), _name);
	bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

	bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_reader, filePath));
	bgfx::setName(handle, _name);

	return handle;
}

bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName)
{
	bgfx::ShaderHandle vsh = loadShader(_reader, _vsName);
	bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
	if (NULL != _fsName)
	{
		fsh = loadShader(_reader, _fsName);
	}

	return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}


static 	bx::FileReaderI* s_fileReader = NULL;
static 	bx::FileWriterI* s_fileWriter = NULL;

bgfx::ProgramHandle loadProgram(const char* _vsName, const char* _fsName)
{

	if (!s_fileReader) {
		s_fileReader = BX_NEW(getDefaultAllocator(), bx::FileReader);
		s_fileWriter = BX_NEW(getDefaultAllocator(), bx::FileWriter);
	}

	return loadProgram(s_fileReader, _vsName, _fsName);
}



bool loadResources() {
	
	PosColorVertex::init();
	PosNormalTexVertex::init();

	if (!s_fileReader) {
		s_fileReader = BX_NEW(getDefaultAllocator(), bx::FileReader);
		s_fileWriter = BX_NEW(getDefaultAllocator(), bx::FileWriter);
	}
	g_resources.vertexColorProgram = loadProgram(s_fileReader, "vs_cubes.sc", "fs_cubes.sc");
	g_resources.meshProgram = loadProgram(s_fileReader, "vs_mesh.sc", "fs_mesh.sc");

	g_resources.teapot.load("assets/teapot.mobj");
	g_resources.sponza.load("assets/NewSponza_Main_Yup_003.fbx");

	g_resources.cube_vbh = bgfx::createVertexBuffer(
		bgfx::makeRef(g_resources.cubeVertices, sizeof(g_resources.cubeVertices)),
		PosColorVertex::s_layout
	);

	g_resources.cube_ibh = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(g_resources.cubeIndexList, sizeof(g_resources.cubeIndexList))
	);

	return true;
}
