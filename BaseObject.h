//set background

#ifndef BASEBJECT_H_
#define BASEOBJECT_H_

#include "commonfunc.h"

class BaseObject
{
	BaseObject();
	~BaseObject();
private:
	SDL_Rect rect_;
	SDL_Texture* p_object;
public:
	virtual bool LoadImg(string path, SDL_Renderer* screen);
	void Render(SDL_Renderer* des, const SDL_Rect* clip = NULL);
	void Free();
	void SetRect(const int& x, const int& y) { rect_.x = x, rect_.y = y; }
	SDL_Rect GetRect() const { return rect_; }
	SDL_Texture* GetObject() const { return p_object; }

};









#endif // BASEOBJECT_H_
