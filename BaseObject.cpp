#include "BaseObject.h"

BaseObject::BaseObject() {
  
	p_object = NULL;
	rect_.x = 0;
	rect_.y = 0;
	rect_.w = 0;
	rect_.h = 0;
}

BaseObject::~BaseObject() { 

}
bool BaseObject::LoadImg(string path, SDL_Renderer* screen) {
	SDL_Texture* new_texture = NULL;
	SDL_Surface* load_surface = IMG_Load(path.c_str());
	if (load_surface != NULL) {
		SDL_SetColorKey(load_surface, SDL_TRUE, SDL_MapRGB(load_surface->format, 167, 175, 180));
		new_texture = SDL_CreateTextureFromSurface(screen, load_surface);
		if (new_texture != NULL) {
			rect_.w = load_surface->w;
			rect_.h = load_surface->h;
		}
		SDL_FreeSurface(load_surface);
	}
	p_object = new_texture;
	return p_object != NULL;
}
void BaseObject::Render(SDL_Renderer* des, const SDL_Rect* clip) {
	SDL_Rect renderquad = { rect_.x, rect_.y, rect_.w, rect_.h };
	if (clip != NULL) {
		renderquad.w = clip->w;
		renderquad.h = clip->h;
	}
	SDL_RenderCopy(des, p_object, clip, &renderquad);
}
void BaseObject::Free() {
	if (p_object != NULL) {
		SDL_DestroyTexture(p_object);
		p_object = NULL;
		rect_.w = 0;
		rect_.h = 0;
	}
}
void BaseObject::SetRect(const int& x, const int& y) {
	rect_.x = x;
	rect_.y = y;
}
SDL_Rect BaseObject::GetRect() const {
	return rect_;
}