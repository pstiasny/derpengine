#include "ResourceManager.h"
#include "../core/Texture.h"
#include "../core/Material.h"
#include "../core/GLSLProgram.h"
#include "../core/GLSLObject.h"
#include "../scene/Tile.h"
#include <list>

ResourceManager * ResourceManager::instance = NULL;

ResourceManager * ResourceManager::getInstance()
{
	if (!instance)
		instance = new ResourceManager;
	return instance;
}

ResourceHandle<GLSLProgram> ResourceManager::getDefaultShaders()
{
	return getShaders("data/default.vs", "data/default.fs");
}

ResourceHandle<GLSLProgram> ResourceManager::getShaders(
		const std::string vertex,
		const std::string fragment)
{
	return glsl_program_pool.getObject(
		std::make_pair(vertex, fragment),
		[&] () {
			std::list< ResourceHandle<GLSLObject> > shaders;
			shaders.push_back(getGLSLProgram(GL_VERTEX_SHADER, vertex.c_str()));
			shaders.push_back(getGLSLProgram(GL_FRAGMENT_SHADER, "data/std.fs"));
			shaders.push_back(getGLSLProgram(GL_FRAGMENT_SHADER, fragment.c_str()));
			return new GLSLProgram(shaders);
		});
}

ResourceHandle<GLSLObject> ResourceManager::getGLSLProgram(
		int type,
		const std::string name)
{
	return glsl_object_pool.getObject(
		std::make_pair(type, name),
		[&] () {
			return new GLSLObject(type, name.c_str());
		});
}

ResourceHandle<Texture> ResourceManager::getTexture(const std::string path)
{
	return texture_pool.getObject(path, [&] () {
			return new Texture(path.c_str());
		});
}

ResourceHandle<Material> ResourceManager::getMaterial(const std::string path)
{
	return material_pool.getObject(path, [&] () {
			ConfigurableMaterial *new_mat = new ConfigurableMaterial;
			new_mat->loadDescriptionFile(path.c_str());
			return new_mat;
		});
}

ResourceHandle<Material> ResourceManager::getDefaultMaterial()
{
	if (!default_material)
		default_material = new Material;
	return default_material;
}

ResourceHandle<Material> ResourceManager::getDefaultDepthMapMaterial()
{
	if (!default_depth_map_material) {
		default_depth_map_material = new Material;
		default_depth_map_material->shaders =
			getShaders("data/default.vs","data/shadowmap.fs");
	}
	return default_depth_map_material;
}

void ResourceManager::clearUnused()
{
	int remaining = 0, deleted = 0;

	if (tile && !tile->hasHandles()) {
		delete tile;
		tile = NULL;
	}

	if (default_material && !default_material->hasHandles())
	{
		delete default_material;
		default_material = NULL;
	}

	if (default_depth_map_material && !default_depth_map_material->hasHandles())
	{
		delete default_depth_map_material;
		default_depth_map_material = NULL;
	}

	int released_materials = material_pool.releaseAllUnused(
		[](const std::string &key, const Material* mat) {
			printf("Material in use: %s\n", key.c_str());
		});
	printf(
		"Released %d materials, %d still in use.\n",
		released_materials, material_pool.size());

	int released_programs = glsl_program_pool.releaseAllUnused(
		[](const std::pair<std::string, std::string> &key, const GLSLProgram* prog) {
			printf("Shader program in use: (%s, %s)\n",
				key.first.c_str(),
				key.second.c_str());
		});
	printf(
		"Released %d programs, %d still in use.\n",
		released_programs, glsl_program_pool.size());

	int released_shader_objects = glsl_object_pool.releaseAllUnused(
		[](const std::pair<int, std::string> &key, const GLSLObject* obj) {
			printf("Shader object in use: %s\n", key.second.c_str());
		});
	printf(
		"Released %d shader objects, %d still in use.\n",
		released_shader_objects, glsl_object_pool.size());

	int released_textures = texture_pool.releaseAllUnused(
		[](const std::string &key, const Texture* tex) {
			printf("Texture in use: %s\n", key.c_str());
		});
	printf(
		"Released %d textures, %d still in use.\n",
		released_textures, texture_pool.size());
}

ResourceHandle<Geometry> ResourceManager::getModel(const std::string name)
{
	// TODO

	if (name == "tile") {
		if (!tile) tile = new Tile();
		return tile;
	}
}

ResourceManager::ResourceManager()
{
	tile = NULL;
	default_material = NULL;
	default_depth_map_material = NULL;
}

ResourceManager::~ResourceManager()
{
	// Currently not called
	clearUnused();
	printf("ResourceManager destroyed \n");
}


