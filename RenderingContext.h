#ifndef RENDERINGCONTEXT_H
#define RENDERINGCONTEXT_H

#include <glm/glm.hpp>

#include "scene/GraphNode.h"
#include "shaders/GLSLProgram.h"

class RenderingContext
{
public:
	RenderingContext(GraphNode *scene);
	~RenderingContext();
	GraphNode* getScene();

	void update();
	void setCamera(glm::vec3 pos, glm::vec3 target);
	void setCamera(glm::vec3 pos, GraphNode *object);
	void reshape(int w, int h);

	void pushMatrix();
	void popMatrix();

	const glm::mat4& getModelMatrix();
	void setModelMatrix(glm::mat4& m);

private:
	glm::mat4 mvp, m_model, m_view, m_projection;
	std::list<glm::mat4> matrix_stack;
	GraphNode *scene;
	GLSLProgram *vertex_shader;

	void updateMatrix();
};

#endif