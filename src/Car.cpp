#include "Car.h"  
#include "Game.h"  

Car::Car(int x, int y) : x(x), y(y), speed(2), texture(nullptr) {
    rect = { x, y, 70, 70 };
}

Car::~Car() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}
void Car::LoadTexture(SDL_Renderer* renderer, const std::string& texturePath) {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        SDL_Log("Failed to load surface for texture: %s, error: %s", texturePath.c_str(), IMG_GetError());
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Failed to create texture from surface: %s, error: %s", texturePath.c_str(), SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Log("Successfully loaded texture: %s", texturePath.c_str());
    SDL_FreeSurface(surface);
}
void Car::SetSpeed(int newSpeed) {
    speed = newSpeed;
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