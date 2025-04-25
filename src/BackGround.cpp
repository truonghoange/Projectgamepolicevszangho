#include "BackGround.h"
#include "Game.h"
#include <SDL_messagebox.h>

BackGround::BackGround(SDL_Renderer* renderer, SDL_Texture* texture) : texture(texture) {
    if (!texture) {
        SDL_Log("Background texture is null!");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Background Texture Failed", "Texture is null", NULL);
    }
    else {
        int texW, texH;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Log("Background texture loaded: width=%d, height=%d", texW, texH);
    }

    y = 0;
}

void BackGround::Update() {
    // Không cần làm gì trong Update
}

void BackGround::Render(SDL_Renderer* renderer) {
    if (!texture) return;

    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    // Lặp background để cuộn liên tục
    int scrollY = Game::cameraY % texH;
    if (scrollY < 0) scrollY += texH;

    SDL_Rect src1 = { 0, 0, texW, texH };
    SDL_Rect dest1 = { 0, -scrollY, Game::SCREEN_WIDTH, texH };
    SDL_RenderCopy(renderer, texture, &src1, &dest1);

    SDL_Rect src2 = { 0, 0, texW, texH };
    SDL_Rect dest2 = { 0, -scrollY + texH, Game::SCREEN_WIDTH, texH };
    SDL_RenderCopy(renderer, texture, &src2, &dest2);

    SDL_Log("Rendering background: cameraY=%d, scrollY=%d, dest1.y=%d, dest2.y=%d",
        Game::cameraY, scrollY, dest1.y, dest2.y);
}