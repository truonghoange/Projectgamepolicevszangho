#pragma once
#include <SDL.h>
#include <SDL_image.h>

class BackGround {
public:
    BackGround(SDL_Renderer* renderer, SDL_Texture* texture); // Thêm tham số texture
    void Update();
    void Render(SDL_Renderer* renderer);

private:
    SDL_Texture* texture;
    int y;
};