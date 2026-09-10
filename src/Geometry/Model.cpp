#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/texture.h>

#include <lumi_render/Geometry/Vec.hpp>
#include <lumi_render/Geometry/Model.hpp>
#include <lumi_render/Geometry/Vertex.hpp>
#include <lumi_render/Texture.hpp>

Model::Model(const std::string& path, bool normalizeToUnitCube) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_PreTransformVertices
	);

	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		throw std::runtime_error("Assimp error: " + std::string(importer.GetErrorString()));
	}

	std::size_t slash = path.find_last_of("/\\");
	directory = (slash == std::string::npos) ? "." : path.substr(0, slash);

	if (normalizeToUnitCube) {
		float maxDistSq = 0.0f;

		for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
			const aiMesh* mesh = scene->mMeshes[m];

			for (unsigned v = 0; v < mesh->mNumVertices; ++v) {
				const aiVector3D& p = mesh->mVertices[v];
				float distSq = p.x * p.x + p.y * p.y + p.z * p.z;

				if (distSq > maxDistSq) maxDistSq = distSq;
			}
		}

		float maxDist = std::sqrt(maxDistSq);
		importScale = (maxDist > 1e-6f) ? (0.5f / maxDist) : 1.0f;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

Vec4 Model::computeTangent(aiMesh* mesh, unsigned i, const Vec3& normal) const {
	Vec3 tangent{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
	Vec3 bitangent{ mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };

	float handedness = Vec3::dot(Vec3::cross(normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;

	return { tangent.x, tangent.y, tangent.z, handedness };
}

void Model::loadMaterialColor(Mesh& result, aiMaterial* material) {
	aiColor4D diffuseColor{1.0f, 1.0f, 1.0f, 1.0f};

	if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == AI_SUCCESS) {
		result.setMaterialColor({ diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a });
	}
}

void Model::loadEmbeddedTexture(Mesh& result, const aiScene* scene, const std::string& pathStr) {
	int texIndex = std::atoi(pathStr.c_str() + 1);

	if (texIndex < 0 || static_cast<unsigned>(texIndex) >= scene->mNumTextures) {
		std::cerr << "Model: embedded texture index out of range: " << pathStr << "\n";
		return;
	}

	const aiTexture* embedded = scene->mTextures[texIndex];

	try {
		if (embedded->mHeight == 0) {
			result.setTexture(Texture::fromMemory(
				reinterpret_cast<const unsigned char*>(embedded->pcData),
				embedded->mWidth
			));
		} else {
			result.setTexture(Texture::fromRawBGRA(
				reinterpret_cast<const unsigned char*>(embedded->pcData),
				static_cast<int>(embedded->mWidth),
				static_cast<int>(embedded->mHeight)
			));
		}
	} catch (const std::exception& e) {
		std::cerr << "Model: " << e.what() << "\n";
	}
}

void Model::loadExternalTexture(Mesh& result, std::string pathStr) {
	for (char& c : pathStr) {
		if (c == '\\') c = '/';
	}

	try {
		result.setTexture(Texture(directory + "/" + pathStr));
	} catch (const std::exception& e) {
		std::cerr << "Model: " << e.what() << "\n";
	}
}

std::vector<Vertex> Model::extractVertices(aiMesh* mesh) const {
	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);

	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		vertices.push_back(extractVertex(mesh, i));
	}

	return vertices;
}

Vertex Model::extractVertex(aiMesh* mesh, unsigned i) const {
	Vertex v{};
	v.position = { mesh->mVertices[i].x * importScale, mesh->mVertices[i].y * importScale, mesh->mVertices[i].z * importScale };

	if (mesh->HasNormals()) v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

	v.texCoord = mesh->mTextureCoords[0] ? Vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y } : Vec2{0.0f, 0.0f};

	if (mesh->HasTangentsAndBitangents()) v.tangent = computeTangent(mesh, i, v.normal);

	if (mesh->HasVertexColors(0)) v.color = { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a };

	return v;
}

std::vector<std::uint32_t> Model::extractIndices(aiMesh* mesh) const {
	std::vector<std::uint32_t> indices;
	indices.reserve(mesh->mNumFaces * 3);

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];

		for (unsigned j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return indices;
}

void Model::loadMaterial(Mesh& result, const aiScene* scene, unsigned materialIndex) {
	if (!scene->mMaterials || materialIndex >= scene->mNumMaterials) return;

	aiMaterial* material = scene->mMaterials[materialIndex];
	loadMaterialColor(result, material);
	loadMaterialTexture(result, scene, material);
}

void Model::loadMaterialTexture(Mesh& result, const aiScene* scene, aiMaterial* material) {
	if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) return;

	aiString texPath;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) != AI_SUCCESS) return;

	std::string pathStr = texPath.C_Str();

	if (!pathStr.empty() && pathStr[0] == '*') {
		loadEmbeddedTexture(result, scene, pathStr);
	} else {
		loadExternalTexture(result, pathStr);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	auto vertices = extractVertices(mesh);
	auto indices = extractIndices(mesh);

	Mesh result(vertices.data(), vertices.size(), indices.data(), indices.size(), Vertex::layout());
	loadMaterial(result, scene, mesh->mMaterialIndex);

	return result;
}
