#pragma once
#include <SDL.h>

class SDLWindowWrapper {
public:

	SDLWindowWrapper(SDL_Window* tex);

	~SDLWindowWrapper();

	SDL_Window* GetTex() const;

	SDL_Window* tex_;
};

class SDLTextureWrapper {
public:

	/** Constructs an ~SDLTextureWrapper */
	SDLTextureWrapper(/** The SDL_Texture to be wrapped */
		SDL_Texture* tex);

	/** Destructs an ~SDLTextureWrapper */
	~SDLTextureWrapper();

	/**
	 * Gets the SDL_Texture of this wrapper.
	 * @return The texture
	 */
	SDL_Texture* GetTex() const;

	/** The wrapped SDL_Texture */
	SDL_Texture* tex_;
};

class SDLRendererWrapper {
public:

	SDLRendererWrapper(SDL_Renderer* tex);

	~SDLRendererWrapper();

	SDL_Renderer* GetTex() const;

	SDL_Renderer* tex_;
};