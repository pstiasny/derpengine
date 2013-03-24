#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

class Texture
{
public:
	Texture(const char* path, bool mipmapped = true);
	virtual ~Texture();
	void use(GLuint unit);

protected:
	Texture() { data = NULL; };
	GLuint texture_id;
	SDL_Surface *data;
};

#endif

