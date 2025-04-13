#pragma once
#include <SDL.h>

class Bullet {
public:
    Bullet(int x, int y);
    void Update();
    void Render(SDL_Renderer* renderer, int cameraY);
    SDL_Rect GetRect() const;
    int GetY() const;
    void MoveBy(int dx, int dy) {
        x += dx;
        y += dy;
        rect.x = x;
        rect.y = y;
    }

private:
    int x, y;
    int speed;
    SDL_Rect rect;
};
