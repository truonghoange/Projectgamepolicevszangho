#include "commonfunc.h"
#include "BaseObject.h"
#include "charobject.h"


CharObject::CharObject()
{
	frame_ = 0;
	x_pos_ = 0;
	y_pos_ = 0;
	x_val_ = 0;
	y_val_ = 0;
	width_frame_ = 0;
	height_frame_ = 0;
	status_ = -1;

}

CharObject::~CharObject() {

}	
bool CharObject::Load_Img(string path, SDL_Renderer* screen) {
	bool ret = BaseObject::Load_Img(path, screen);
	if (ret == true) {
		width_frame_ = rect_.w / 6;
		height_frame_ = rect_.h;
	}
	return ret;

}
void CharObject::HandleInputAction(SDL_Event events, SDL_Renderer* screen) {
	if (events.type == SDL_KEYDOWN) {
		switch (events.key.keysym.sym) {
		case SDLK_UP:
			y_val_ -= 10;
			break;
		case SDLK_DOWN:
			y_val_ += 10;
			break;
		case SDLK_LEFT:
			x_val_ -= 10;
			break;
		case SDLK_RIGHT:
			x_val_ += 10;
			break;
		}
	}
	else if (events.type == SDL_KEYUP) {
		switch (events.key.keysym.sym) {
		case SDLK_UP:
			y_val_ += 10;
			break;
		case SDLK_DOWN:
			y_val_ -= 10;
			break;
		case SDLK_LEFT:
			x_val_ += 10;
			break;
		case SDLK_RIGHT:
			x_val_ -= 10;
			break;
		}
	}
}
void CharObject::set_clips() {
	if (width_frame_ > 0 && height_frame_ > 0) {
		frame_clip_[0].x = 0;
		frame_clip_[0].y = 0;
		frame_clip_[0].w = width_frame_;
		frame_clip_[0].h = height_frame_;
		frame_clip_[1].x = width_frame_;
		frame_clip_[1].y = 0;
		frame_clip_[1].w = width_frame_;
		frame_clip_[1].h = height_frame_;
		frame_clip_[2].x = 2 * width_frame_;
		frame_clip_[2].y = 0;
		frame_clip_[2].w = width_frame_;
		frame_clip_[2].h = height_frame_;
		frame_clip_[3].x = 3 * width_frame_;
		frame_clip_[3].y = 0;
		frame_clip_[3].w = width_frame_;
		frame_clip_[3].h = height_frame_;
		frame_clip_[4].x = 4 * width_frame_;
		frame_clip_[4].y = 0;
		frame_clip_[4].w = width_frame_;
		frame_clip_[4].h = height_frame_;
		frame_clip_[5].x = 5 * width_frame_;
		frame_clip_[5].y = 0;
		frame_clip_[5].w = width_frame_;
		frame_clip_[5].h = height_frame_;
	}
}
void CharObject::Show(SDL_Renderer* des) {
	if (x_val_ == 0 && y_val_ == 0) {
		frame_ = 0;
	}
	else {
		frame_++;
	}
	if (frame_ >= 6) {
		frame_ = 0;
	}
	if (x_val_ > 0) {
		status_ = WALK_RIGHT;
	}
	else if (x_val_ < 0) {
		status_ = WALK_LEFT;
	}
	if (status_ == WALK_LEFT) {
		Load_Img("image//walk_left.png", des);
	}
	else {
		Load_Img("image//walk_right.png", des);
	}
	rect_.x = x_pos_;
	rect_.y = y_pos_;
	SDL_Rect* current_clip = &frame_clip_[frame_];
	SDL_Rect renderQuad = { rect_.x, rect_.y, width_frame_, height_frame_ };
	SDL_RenderCopy(des, p_object, current_clip, &renderQuad);
}