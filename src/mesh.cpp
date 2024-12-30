#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "vertex.h"

Group::Group() {
	reset();
}

void Group::reset() {
	m_vbh = BGFX_INVALID_HANDLE;
	m_ibh = BGFX_INVALID_HANDLE;
	m_numVertices = 0;
	m_vertices = nullptr;
	m_numIndices = 0;
	m_indices = nullptr;
}

void Mesh::load(const std::string& filepath) {
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// Load the OBJ file with some post-processing flags
	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_Triangulate |         // Ensure all faces are triangles
		aiProcess_JoinIdenticalVertices | // Merge duplicate vertices
		aiProcess_GenNormals |           // Generate normals if not present
		aiProcess_CalcTangentSpace);     // Calculate tangent and bitangent

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("Error: %s\n", importer.GetErrorString());
		return;
	}

	printf("Successfully loaded: %s\n", filepath.c_str());

	// Process all meshes in the scene
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		Group group;

		aiMesh* mesh = scene->mMeshes[i];



		// Process vertices
		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			PosNormalTexVertex vertex;
			vertex.m_position[0] = mesh->mVertices[j].x;
			vertex.m_position[1] = mesh->mVertices[j].y;
			vertex.m_position[2] = mesh->mVertices[j].z;

			//vertex.m_abgr = 0xFFFFFFFF;

			if (mesh->HasNormals()) {
				vertex.m_normal[0] = mesh->mNormals[j].x;
				vertex.m_normal[1] = mesh->mNormals[j].y;
				vertex.m_normal[2] = mesh->mNormals[j].z;
			}

			if (mesh->mTextureCoords[0]) { // Check if texture coordinates exist
				vertex.m_texCoords[0] = mesh->mTextureCoords[0][j].x;
				vertex.m_texCoords[1] = mesh->mTextureCoords[0][j].y;
			}
			else {
				vertex.m_texCoords[0] = 0.0f;
				vertex.m_texCoords[1] = 0.0f;
			}

			vertices.push_back(vertex);
		}

		group.m_numVertices = vertices.size();
		group.m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size() * sizeof(PosNormalTexVertex)), PosNormalTexVertex::ms_layout);

		// Process indices
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++) {
				indices.push_back(face.mIndices[k]);
			}
		}

		group.m_numIndices = indices.size();
		group.m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), indices.size() * sizeof(u16)));
		
		m_groups.push_back(group);
		// Output mesh data
		printf("Mesh %d: \n", i);
		printf(" - Vertices: %zu\n", vertices.size());
		printf(" - Indices: %zu\n", indices.size());
	}
}

void Mesh::submit(bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state) const
{
	if (BGFX_STATE_MASK == _state)
	{
		_state = 0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA
			;
	}

	bgfx::setTransform(_mtx);
	bgfx::setState(_state);

	for (std::vector<Group>::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd; ++it)
	{
		const Group& group = *it;

		bgfx::setIndexBuffer(group.m_ibh);
		bgfx::setVertexBuffer(0, group.m_vbh);
		bgfx::submit(
			_id
			, _program
			, 0
			, BGFX_DISCARD_INDEX_BUFFER
			| BGFX_DISCARD_VERTEX_STREAMS
		);
	}

	bgfx::discard();
}

