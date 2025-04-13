#include "Bullet.h"

Bullet::Bullet(int x, int y) : x(x), y(y) {
    speed = 10; // Tốc độ đạn, có thể điều chỉnh
    rect = { x, y, 10, 10 }; // Kích thước đạn
}

void Bullet::Update() {
    y -= speed; // Đạn bay lên
    rect.y = y;
	rect.x = x;
}

void Bullet::Render(SDL_Renderer* renderer, int cameraY) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu cho đạn
    SDL_Rect renderRect = { rect.x, rect.y - cameraY, rect.w, rect.h }; // Điều chỉnh theo cameraY
    SDL_RenderFillRect(renderer, &renderRect);
}

SDL_Rect Bullet::GetRect() const {
    return rect;
}

int Bullet::GetY() const {
    return y;
}