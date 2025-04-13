#include"Car.h"
#include"Game.h"

Car::Car(int x, int y) : x(x), y(y), speed(5) {
    rect = { x, y, 50, 100 }; // width, height của xe
    color = { 255, 255, 255, 255 }; // mặc định: trắng
}

Car::~Car() {}

void Car::Render(SDL_Renderer* renderer, int cameraY) {
    SDL_Rect drawRect = rect;
    drawRect.y -= cameraY; // Điều chỉnh theo cameraY  
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &drawRect);
}

SDL_Rect Car::GetRect() const {
    return rect;
}