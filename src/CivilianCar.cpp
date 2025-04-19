#include "CivilianCar.h"
#include "Game.h"

CivilianCar::CivilianCar(int x, int y) : Car(x, y), carType(0), isExploding(false), explosionFrame(0),
explosionFrameWidth(38), explosionFrameHeight(32), explosionTotalFrames(5),
lastExplosionUpdate(0), explosionTexture(nullptr)
{
    color = { 0, 255, 0, 255 }; // Giữ màu nhưng không dùng để vẽ
    speed = 2;

    // Load texture nổ
    explosionTexture = IMG_LoadTexture(Game::renderer, "assets/explosion.png");
    if (!explosionTexture) {
        SDL_Log("Failed to load explosion texture for civilian: %s", IMG_GetError());
    }

    // Chọn ngẫu nhiên loại xe
    carType = rand() % 6; // 0: Xe vàng, 1: Xe xanh lá, 2: Xe tải, 3: Xe xanh dương
    if (carType == 0) {
        LoadTexture(Game::renderer, "assets/JeepB.png");
        rect.w = 42;
        rect.h = 83;
        SDL_Log("Creating civilian car, carType=%d (yellow)", carType);
    }
    else if (carType == 1) {
        LoadTexture(Game::renderer, "assets/RamB.png");
        rect.w = 39;
        rect.h = 87;
        SDL_Log("Creating civilian car, carType=%d (yellow)", carType);
    }
    else if (carType == 2) {
        LoadTexture(Game::renderer, "assets/cars_silver.png");
        rect.w = 50;
        rect.h = 90; // Xe tải dài hơn
        SDL_Log("Creating civilian car, carType=%d (truck)", carType);
    }
    else if (carType == 3) {
        LoadTexture(Game::renderer, "assets/civilian1.png");
        rect.w = 50;
        rect.h = 70;
        SDL_Log("Creating civilian car, carType=%d (blue)", carType);
    }
    else if (carType == 4) {
        LoadTexture(Game::renderer, "assets/BuickerB.png");
        rect.w = 50;
        rect.h = 112;
        SDL_Log("Creating civilian car, carType=%d (blue)", carType);
    }
    else {
        LoadTexture(Game::renderer, "assets/SuperB.png");
        rect.w = 40;
        rect.h = 79;
        SDL_Log("Creating civilian car, carType=%d (blue)", carType);
    }

    // Kiểm tra ngay sau khi load texture
    if (!texture) {
        SDL_Log("Failed to load texture for civilian car at creation, carType=%d", carType);
    }
}

CivilianCar::~CivilianCar() {
    if (explosionTexture) {
        SDL_DestroyTexture(explosionTexture);
        explosionTexture = nullptr;
    }
}

void CivilianCar::StartExplosion() {
    isExploding = true;
    lastExplosionUpdate = SDL_GetTicks();
    SDL_Log("Civilian car started exploding");
}

void CivilianCar::Update() {
    if (isExploding) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastExplosionUpdate >= 100) { // Chuyển frame mỗi 100ms
            explosionFrame++;
            lastExplosionUpdate = currentTime;
        }
        return;
    }

    y += speed;
    rect.x = x;
    rect.y = y;
}

SDL_Rect CivilianCar::GetRect() const {
    return rect;
}

void CivilianCar::Render(SDL_Renderer* renderer, int cameraY) {
    if (isExploding) {
        if (explosionFrame < explosionTotalFrames && explosionTexture) {
            SDL_Rect srcRect = { explosionFrame * explosionFrameWidth, 0, explosionFrameWidth, explosionFrameHeight };
            SDL_Rect dstRect = {
                rect.x + (rect.w - explosionFrameWidth) / 2,
                rect.y - cameraY + (rect.h - explosionFrameHeight) / 2,
                explosionFrameWidth, explosionFrameHeight
            };
            SDL_RenderCopy(renderer, explosionTexture, &srcRect, &dstRect);
        }
        return;
    }
    if (!texture) {
        SDL_Log("No texture for civilian car, carType=%d", carType);
        return; // Không vẽ gì nếu texture không load được
    }

    SDL_Rect drawRect = rect;
    drawRect.y -= cameraY;
    SDL_RenderCopy(renderer, texture, nullptr, &drawRect);
}