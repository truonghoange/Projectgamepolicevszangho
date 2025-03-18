#pragma once
#include"commonfunc.h"
#include"BaseObject.h"

class CharObject:public BaseObject{
public:
	CharObject();
	~CharObject();
	
	enum WalkType
	{
		WALK_RIGHT = 0,
		WALK_LEFT = 1,
		WALK_UP = 2,
		WALK_DOWN = 3,
	};

	bool Load_Img(string path, SDL_Renderer* screen);
	
	void Show(SDL_Renderer* des);
	void HandleInputAction(SDL_Event events, SDL_Renderer* screen);
	void set_clips();
private:
	int x_val_;
	int y_val_;
	int x_pos_;
	int y_pos_;

	int width_frame_;
	int height_frame_;
	
	SDL_Rect frame_clip_[6];


	Input input_type_;

	int frame_;
	int status_;


};