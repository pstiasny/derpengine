#include <algorithm>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "GraphNode.h"
#include "RenderingContext.h"

GraphNode::GraphNode()
{
	parent = NULL;
	visible = true;
	scale.v[0] = scale.v[1] = scale.v[2] = 1.0f;
	m_transform = glm::mat4(1.0f);
}

/// Scene graph manages its memory.
GraphNode::~GraphNode()
{
	if (parent) parent->members.remove(this);

	for (std::list<GraphNode*>::iterator it = members.begin();
			it != members.end(); it++) {
		(*it)->parent = NULL;
		delete *it;
	}
}

/// Template Method that sets up transformations and calls the actual
/// object and member rendering.
void GraphNode::render(RenderingContext *rc)
{
	if (!visible) return;

	beforeRender(rc);

	// Apply transformations
	if (pos.isSet() || rot.isSet() || !scale.isOnes()) {
		rc->pushMatrix();
		rc->setModelMatrix(rc->getModelMatrix() * m_transform);
	}

	// Call concrete rendering implementation
	doRender(rc);

	// Render members
	std::for_each(members.begin(), members.end(),
			std::bind2nd(std::mem_fun(&GraphNode::render), rc));

	// Revert transformations
	if (pos.isSet() || rot.isSet() || !scale.isOnes()) rc->popMatrix();

	afterRender(rc);
}

/// Same as render, but renders a material-less version for the depth buffer
/// only
void GraphNode::depthRender(RenderingContext *rc)
{
	if (!visible) return;

	if (pos.isSet() || rot.isSet() || !scale.isOnes()) {
		rc->pushMatrix();
		rc->setModelMatrix(rc->getModelMatrix() * m_transform);
	}

	doDepthRender(rc);

	std::for_each(members.begin(), members.end(),
			std::bind2nd(std::mem_fun(&GraphNode::depthRender), rc));

	if (pos.isSet() || rot.isSet() || !scale.isOnes()) rc->popMatrix();
}

/// Concrete implementation of node's rendering.
///
/// Base implementation is a no-op.
void GraphNode::doRender(RenderingContext *rc)
{
}

void GraphNode::beforeRender(RenderingContext *rc)
{
}

void GraphNode::afterRender(RenderingContext *rc)
{
}

void GraphNode::doDepthRender(RenderingContext *rc)
{
	doRender(rc);
}

void GraphNode::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	pos.v[0] = x;
	pos.v[1] = y;
	pos.v[2] = z;
	updateTransformMatrix();
}

void GraphNode::setPosition(const glm::vec3& pos)
{
	this->pos.v[0] = pos.x;
	this->pos.v[1] = pos.y;
	this->pos.v[2] = pos.z;
	updateTransformMatrix();
}

void GraphNode::setRotation(GLfloat x, GLfloat y, GLfloat z)
{
	rot.v[0] = x;
	rot.v[1] = y;
	rot.v[2] = z;
	updateTransformMatrix();
}

void GraphNode::setScale(GLfloat x, GLfloat y, GLfloat z)
{
	scale.v[0] = x;
	scale.v[1] = y;
	scale.v[2] = z;
	updateTransformMatrix();
}

void GraphNode::updateTransformMatrix()
{
	m_transform = glm::mat4(1.0f);
	if (pos.isSet()) {
		m_transform = glm::translate(m_transform, pos.toGLM());
	}
	if (rot.isSet()) {
		if(rot.v[0] != 0.0f)
			m_transform = glm::rotate(m_transform, glm::radians(rot.v[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		if(rot.v[1] != 0.0f)
			m_transform = glm::rotate(m_transform, glm::radians(rot.v[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		if(rot.v[2] != 0.0f)
			m_transform = glm::rotate(m_transform, glm::radians(rot.v[2]), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (!scale.isOnes()) {
		m_transform = glm::scale(m_transform, scale.toGLM());
	}
}

glm::vec4 GraphNode::getWorldCoordinates(const glm::vec4& v)
{
	if (parent == NULL)
		return getRelativeCoordinates(v);
	else
		return parent->getWorldCoordinates(getRelativeCoordinates(v));
}

glm::vec4 GraphNode::getWorldCoordinates()
{
	glm::vec4 zero(0.0f, 0.0f, 0.0f, 1.0f);
	return getWorldCoordinates(zero);
}

glm::vec4 GraphNode::getRelativeCoordinates(const glm::vec4& v)
{
	return m_transform * v;
}

void GraphNode::addMember(GraphNode* member)
{
	member->parent = this;
	members.push_back(member);
}

void GraphNode::removeMember(GraphNode* member)
{
	delete member;
}

void GraphNode::setVisibility(bool v)
{
	visible = v;
}
