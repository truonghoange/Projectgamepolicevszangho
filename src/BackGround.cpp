#include "BackGround.h"
#include "Game.h"
#include <SDL_messagebox.h> // cần cho SDL_ShowSimpleMessageBox

BackGround::BackGround(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("image/road.png");

    if (!surface) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "IMG_Load Failed", IMG_GetError(), NULL);
        texture = nullptr;
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CreateTexture Failed", SDL_GetError(), NULL);
    }

    y = 0;
}
void BackGround::Update() {}
void BackGround::Render(SDL_Renderer* renderer) {
    if (!texture) return;

    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    // Xử lý cameraY âm và dương
    int scrollY = Game::cameraY % texH;
    if (scrollY < 0) scrollY += texH; // Đảm bảo scrollY luôn dương

    // Vẽ phần chính từ scrollY đến cuối texture
    SDL_Rect src1 = { 0, scrollY, texW, texH - scrollY };
    SDL_Rect dest1 = { 0, 0, Game::SCREEN_WIDTH, texH - scrollY };
    SDL_RenderCopy(renderer, texture, &src1, &dest1);

    // Vẽ phần còn lại từ đầu texture đến scrollY
    if (scrollY > 0) {
        SDL_Rect src2 = { 0, 0, texW, scrollY };
        SDL_Rect dest2 = { 0, texH - scrollY, Game::SCREEN_WIDTH, scrollY };
        SDL_RenderCopy(renderer, texture, &src2, &dest2);
    }
}