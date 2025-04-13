#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
class Car {
public:
    Car(int x, int y);
    virtual ~Car();

    virtual void Update() = 0;
    virtual void Render(SDL_Renderer* renderer, int cameraY);

    SDL_Rect GetRect() const;
    void MoveBy(int dx, int dy) {
        x += dx;
        y += dy;
        rect.x = x;
        rect.y = y;
    }


protected:
    int x, y;
    int speed;
    SDL_Rect rect;
    SDL_Color color;
};
