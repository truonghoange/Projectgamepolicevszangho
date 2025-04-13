#include "PoliceCar.h"
#include "Game.h"
#include <vector>
#include "Bullet.h"

PoliceCar::PoliceCar(int x, int y) : Car(x, y) {
    color = { 0, 0, 255, 255 }; // Màu xanh (cảnh sát)
    speed = 3 ; //  tốc độ
}

void PoliceCar::HandleInput(const Uint8* keystates) {
    // Di chuyển xe cảnh sát dựa trên vị trí thực tế (không bị giới hạn bởi màn hình tĩnh)
    if (keystates[SDL_SCANCODE_LEFT] && x > 0) x -= speed;
    if (keystates[SDL_SCANCODE_RIGHT] && x < Game::SCREEN_WIDTH - rect.w) x += speed;

    // Di chuyển lên/xuống dựa trên cameraY để phản ánh đúng vị trí background
    if (keystates[SDL_SCANCODE_UP]) y -= speed;
    if (keystates[SDL_SCANCODE_DOWN]) y += speed;

    rect.x = x;
    rect.y = y;

    // Bắn đạn
    if (keystates[SDL_SCANCODE_SPACE] && canShoot) {
        Shoot();
        canShoot = false;
    }
    else if (!keystates[SDL_SCANCODE_SPACE]) {
        canShoot = true;
    }
}

void PoliceCar::Update() {
    // Cập nhật đạn
    for (auto bullet : bullets) {
        bullet->Update();
    }

    
}

void PoliceCar::Render(SDL_Renderer* renderer, int cameraY) {
    // Sửa: Truyền cameraY vào để xe cảnh sát di chuyển theo background
    Car::Render(renderer, cameraY);
    for (auto bullet : bullets) {
        bullet->Render(renderer, cameraY);
    }
}

std::vector<Bullet*>& PoliceCar::GetBullets() {
    return bullets;
}

void PoliceCar::Shoot() {
    
    int bulletX = x + rect.w / 2 - 5;
    int bulletY = y - 10 ;
    bullets.push_back(new Bullet(bulletX, bulletY));
}