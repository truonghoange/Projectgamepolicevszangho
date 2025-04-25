#include "PoliceCar.h"
#include "Game.h"
#include <vector>
#include "Bullet.h"
#include "SoundManager.h"
PoliceCar::PoliceCar(int x, int y) : Car(x, y), canShoot(true), lastShotTime(0), sirenOn(true), lastSirenUpdate(0), sirenRed(true),
currentFrame(0), frameWidth(50), frameHeight(50), totalFrames(3),
isExploding(false), explosionFrame(0), explosionFrameWidth(38), explosionFrameHeight(32),
explosionTotalFrames(5), lastExplosionUpdate(0), explosionTexture(nullptr)
{
    color = { 0, 0, 255, 255 };
    
    LoadTexture(Game::renderer, "assets/PoliceCar.png"); // Load sprite sheet
    explosionTexture = IMG_LoadTexture(Game::renderer, "assets/explosion.png");
    if (!explosionTexture) {
        SDL_Log("Failed to load explosion texture for police: %s", IMG_GetError());
    }
    rect.w = 50;
    rect.h = 70;
}

PoliceCar::~PoliceCar() {
    for (auto bullet : bullets) delete bullet;
    bullets.clear();
    if (explosionTexture) {
        SDL_DestroyTexture(explosionTexture);
        explosionTexture = nullptr;
    }
}

void PoliceCar::StartExplosion() {
    isExploding = true;
    lastExplosionUpdate = SDL_GetTicks();
    SDL_Log("Police car started exploding");
}

void PoliceCar::HandleInput(const Uint8* keystates) {
    if (isExploding) return; // Không xử lý input khi đang nổ

    if (keystates[SDL_SCANCODE_A] && x > 0) {
        x -= speed;
        SDL_Log("Police move left: x=%d", x);
    }
    if (keystates[SDL_SCANCODE_D] && x < Game::SCREEN_WIDTH - rect.w) {
        x += speed;
        SDL_Log("Police move right: x=%d", x);
    }
    if (keystates[SDL_SCANCODE_W]) {
        y -= speed;
        SDL_Log("Police move up: y=%d", y);
    }
    if (keystates[SDL_SCANCODE_S]) {
        y += speed;
        SDL_Log("Police move down: y=%d", y);
    }

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

    if (keystates[SDL_SCANCODE_J] && canShoot) {
        Shoot();
        canShoot = false;
    }
    else if (!keystates[SDL_SCANCODE_J]) {
        canShoot = true;
    }
}

void PoliceCar::Update() {
    if (isExploding) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastExplosionUpdate >= 300) { // Chuyển frame mỗi 300ms
            explosionFrame++;
            lastExplosionUpdate = currentTime;
        }
        return;
    }


    rect.x = x;
    rect.y = y;

    for (auto bullet : bullets) {
        bullet->Update();
    }
}

void PoliceCar::Render(SDL_Renderer* renderer, int cameraY) {
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

    // Cập nhật frame cho siren
    if (sirenOn) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastSirenUpdate >= 500) {
            sirenRed = !sirenRed;
            currentFrame = sirenRed ? 0 : 2;
            lastSirenUpdate = currentTime;
        }
    }
    else {
        currentFrame = 1;
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
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShotTime >= 500) { // Giới hạn tần suất bắn
        Bullet* bullet = new Bullet(rect.x + rect.w / 2 - 5, rect.y - 20);
        bullets.push_back(bullet);
        lastShotTime = currentTime;
        SDL_Log("Police car shot a bullet at x=%d, y=%d", bullet->GetRect().x, bullet->GetRect().y);
        SoundManager::GetInstance().PlayGunshot(); // Phát âm thanh tiếng súng
    }
}