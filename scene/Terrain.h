#include "GraphNode.h"

class Terrain : public GraphNode
{
public:
	Terrain(const char *hmap_path, GLfloat vertical_scaling);
	~Terrain();
	virtual void doRender(RenderingContext *rc);
	GLfloat getHeight(GLfloat x, GLfloat z);

private:
	GLfloat *vertex_data, *normal_data, *uv_data;
	GLuint *index_data;
	GLint x_res, z_res;
	GLfloat vertical_scaling;
};

