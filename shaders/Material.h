#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

#include "GLSLProgramPool.h"

/// A Material represents a set of parameters for the shader program.
// This can be extended for specific shaders.
class Material
{
public:
	Material();
	~Material();
	virtual void use();
	GLSLProgram * getShaders() { return shaders; };

	glm::vec4 ambient, diffuse;
	GLSLProgram *shaders;
};

#endif

