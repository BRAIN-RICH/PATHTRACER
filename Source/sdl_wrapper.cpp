#include "sdl_wrapper.h"
//texture
SDLTextureWrapper::SDLTextureWrapper(SDL_Texture* tex) : tex_(tex) {}

SDLTextureWrapper::~SDLTextureWrapper() { SDL_DestroyTexture(tex_); }

SDL_Texture* SDLTextureWrapper::GetTex() const { return tex_; }

//window
SDLWindowWrapper::SDLWindowWrapper(SDL_Window* tex) : tex_(tex) {}

SDLWindowWrapper::~SDLWindowWrapper() { SDL_DestroyWindow(tex_); }

SDL_Window* SDLWindowWrapper::GetTex() const { return tex_; }

//window
SDLRendererWrapper::SDLRendererWrapper(SDL_Renderer* tex) : tex_(tex) {}

SDLRendererWrapper::~SDLRendererWrapper() { SDL_DestroyRenderer(tex_); }

SDL_Renderer* SDLRendererWrapper::GetTex() const { return tex_; }