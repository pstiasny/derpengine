#include <GL/glew.h>

#include "Tile.h"

const GLfloat Tile::vertex_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f
};

const GLfloat Tile::uv_data[] = {
	0.0f, 1.0f, 
	1.0f, 1.0f, 
	1.0f, 0.0f, 
	0.0f, 0.0f, 
};

const GLushort Tile::index_data[] = {
	0, 2, 1,
	0, 3, 2
};

Tile::Tile(const char *texture_path)
{
	if (texture_path) {
		tex = new Texture(texture_path, false);
		tex->use(0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else
		tex = NULL;
}

Tile::~Tile()
{
	if (tex) delete tex;
}

void Tile::doRender(RenderingContext *rc)
{
	if (tex) tex->use(0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex_data);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, uv_data);

	//glDrawArrays(GL_TRIANGLES, 0, 12*3);
	glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_SHORT, index_data);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

