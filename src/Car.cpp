#include "Car.h"  
#include "Game.h"  

Car::Car(int x, int y) : x(x), y(y), speed(2), texture(nullptr) {
    rect = { x, y, 50, 70 };
}

Car::~Car() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}
void Car::LoadTexture(SDL_Renderer* renderer, const std::string& texturePath) {
	SDL_Surface* surface = IMG_Load(texturePath.c_str());
	if (!surface) {
		SDL_Log("Failed to load image: %s", IMG_GetError());
		return;
	}
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
}
void Car::Render(SDL_Renderer* renderer, int cameraY) {
    // Chỉ vẽ bánh xe (texture đã được vẽ trong PoliceCar::Render)
    SDL_Rect drawRect = rect;
    drawRect.y -= cameraY;

    

    
    
}

// Cập nhật vị trí của xe

SDL_Rect Car::GetRect() const {  
   return rect;  
}