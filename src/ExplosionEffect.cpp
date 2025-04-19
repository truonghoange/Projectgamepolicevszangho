#include "ExplosionEffect.h"
#include"Game.h"

ExplosionEffect::ExplosionEffect(int x, int y, SDL_Renderer* renderer) : x(x), y(y), frameWidth(38), frameHeight(32), totalFrames(5), currentFrame(0), lastFrameUpdate(0) {
    texture = IMG_LoadTexture(renderer, "assets/explosion.png");
    if (!texture) {
        SDL_Log("Failed to load explosion small texture: %s", IMG_GetError());
    }
}

ExplosionEffect::~ExplosionEffect() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void ExplosionEffect::Update() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFrameUpdate >= 1000) { // Chuyển frame mỗi 1000ms
        currentFrame++;
        lastFrameUpdate = currentTime;
    }
}

void ExplosionEffect::Render(SDL_Renderer* renderer, int cameraY) {
    if (currentFrame >= totalFrames) return;

    SDL_Rect srcRect = { currentFrame * frameWidth, 0, frameWidth, frameHeight };
    SDL_Rect dstRect = { x, y - cameraY, frameWidth, frameHeight };
    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
}