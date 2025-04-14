#include "PoliceCar.h"
#include "Game.h"
#include <vector>
#include "Bullet.h"

PoliceCar::PoliceCar(int x, int y) : Car(x, y), canShoot(true), lastShotTime(0), sirenOn(true), lastSirenUpdate(0), sirenRed(true), currentFrame(0), frameWidth(50), frameHeight(50), totalFrames(3) {
    color = { 0, 0, 255, 255 };
    speed = 3;
    LoadTexture(Game::renderer, "assets/PoliceCar.png"); // Load sprite sheet
}

PoliceCar::~PoliceCar() {
    for (auto bullet : bullets) delete bullet;
    bullets.clear();
}

void PoliceCar::HandleInput(const Uint8* keystates) {
    if (keystates[SDL_SCANCODE_LEFT] && x > 0) x -= speed;
    if (keystates[SDL_SCANCODE_RIGHT] && x < Game::SCREEN_WIDTH - rect.w) x += speed;
    if (keystates[SDL_SCANCODE_UP]) y -= speed;
    if (keystates[SDL_SCANCODE_DOWN]) y += speed;

    // Bật/tắt siren khi nhấn phím S
    if (keystates[SDL_SCANCODE_S]) {
        static Uint32 lastToggle = 0;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastToggle > 300) {
            ToggleSiren();
            lastToggle = currentTime;
            SDL_Log("Siren %s", sirenOn ? "ON" : "OFF");
        }
    }

    rect.x = x;
    rect.y = y;

    if (keystates[SDL_SCANCODE_SPACE] && canShoot) {
        Shoot();
        canShoot = false;
    }
    else if (!keystates[SDL_SCANCODE_SPACE]) {
        canShoot = true;
    }
}

void PoliceCar::Update() {
    for (auto bullet : bullets) {
        bullet->Update();
    }
}

void PoliceCar::Render(SDL_Renderer* renderer, int cameraY) {
    // Cập nhật frame cho siren
    if (sirenOn) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastSirenUpdate >= 500) { // Nhấp nháy mỗi 500ms
            sirenRed = !sirenRed;
            currentFrame = sirenRed ? 0 : 1; // Frame 0 (đỏ), Frame 1 (xanh)
            lastSirenUpdate = currentTime;
        }
    }
    else {
        currentFrame = 2; // Frame 2 (không đèn)
    }

    // Vẽ xe với frame hiện tại
    SDL_Rect drawRect = rect;
    drawRect.y -= cameraY;
    if (texture) {
        SDL_Rect srcRect = { currentFrame * frameWidth, 0, frameWidth, frameHeight };
        SDL_RenderCopy(renderer, texture, &srcRect, &drawRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &drawRect);
    }
     
    
   

    // Vẽ đạn
    for (auto bullet : bullets) {
        bullet->Render(renderer, cameraY);
    }
}

std::vector<Bullet*>& PoliceCar::GetBullets() {
    return bullets;
}

void PoliceCar::Shoot() {
    int bulletX = x + rect.w / 2 - 5;
    int bulletY = y - 10;
    bullets.push_back(new Bullet(bulletX, bulletY));
}