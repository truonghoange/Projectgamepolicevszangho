#include "CriminalCar.h"
#include <algorithm>
#include <cmath>

CriminalCar::CriminalCar(int x, int y) : Car(x, y) {
    color = { 255, 0, 0, 255 }; // Màu ?? (t?i ph?m)
    speed = 1;
}

void CriminalCar::SetCivilianCars(const std::vector<Car*>& civilians) {
    civilianCars = civilians;
}

void CriminalCar::Update() {
    // Di chuy?n ti?n lên
    y -= speed;

    // Né xe dân
    for (auto civ : civilianCars) {
        SDL_Rect civRect = civ->GetRect();
        if (SDL_HasIntersection(&rect, &civRect)) {
            if (x + rect.w < 800) x += speed * 2; // né sang ph?i n?u còn ch?
            else x -= speed * 2; // né sang trái n?u h?t ch?
        }
    }
    if (x < 0) x = 0;
   
    if (x > 800 - rect.w) x = 800 - rect.w;

    rect.x = x;
    rect.y = y;
}
void CriminalCar::TakeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    SDL_Log("[Damage] HP: %d (Damage: %d)", hp, dmg); // Thêm log chi tiết debug
}
bool CriminalCar::IsDead() const {
	return hp <= 0;
}
void CriminalCar::Render(SDL_Renderer* renderer, int cameraY) {
    Car::Render(renderer, cameraY);

    // Tính toán vị trí thanh máu dựa trên cameraY
    SDL_Rect hpBarBack = {
        rect.x,
        (rect.y - cameraY) - 10, // Điều chỉnh theo cameraY
        rect.w,
        5
    };
    SDL_Rect hpBarFront = {
        rect.x,
        (rect.y - cameraY) - 10,
        static_cast<int>(rect.w * ((float)hp / maxHp)),
        5
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarBack);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarFront);
}