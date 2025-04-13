#include "CriminalCar.h"
#include <algorithm>
#include <cmath>

CriminalCar::CriminalCar(int x, int y) : Car(x, y) {
    color = { 255, 0, 0, 255 };
    speed = 1;
}

void CriminalCar::SetCivilianCars(const std::vector<Car*>& civilians) {
    civilianCars = civilians;
}

void CriminalCar::Update() {
    y -= speed;
    rect.y = y;
    SDL_Log("Criminal y=%d", y);

    static float smoothedForceX = 0.0f; // Lưu lực mịn qua các frame
    float avoidForceX = 0.0f;
    bool shouldAvoid = false;
    const float AVOID_RANGE_Y = 400.0f;
    const float AVOID_RANGE_X = 150.0f;
    const float AVOID_SPEED = speed * 3.0f; // Giảm để mượt hơn
    const float CLOSE_RANGE = 150.0f; // Tăng để né sớm hơn
    const float DAMPING = 0.7f; // Giảm dao động
    const float MAX_FORCE = 10.0f; // Giới hạn lực để tránh giật

    // Đếm xe dân ở hai bên để ưu tiên hướng né
    int leftCount = 0, rightCount = 0;

    for (auto civ : civilianCars) {
        if (!civ) continue;
        int civX = civ->GetRect().x;
        int civY = civ->GetRect().y;
        int distanceY = y - civY;
        int distanceX = x - civX;

        if (distanceY > 0 && distanceY < AVOID_RANGE_Y) {
            if (std::abs(distanceX) < AVOID_RANGE_X) {
                // Tính weight dựa trên cả Y và X
                float weight = (1.0f - (distanceY / AVOID_RANGE_Y)) * (1.0f - (std::abs(distanceX) / AVOID_RANGE_X));
                if (distanceY < CLOSE_RANGE) weight *= 3.0f; // Né gấp
                SDL_Log("Criminal avoiding civilian at x=%d, y=%d, distanceY=%d, distanceX=%d, weight=%f", civX, civY, distanceY, distanceX, weight);
                shouldAvoid = true;
                // Lực né mạnh hơn nếu xe dân gần
                float force = AVOID_SPEED * weight * (distanceX > 0 ? -1.0f : 1.0f);
                avoidForceX += force;

                // Đếm để ưu tiên hướng
                if (distanceX > 0) leftCount++;
                else rightCount++;
            }
        }
    }

    if (shouldAvoid) {
        // Ưu tiên hướng né có ít xe dân hơn
        if (leftCount > rightCount && avoidForceX < 0) {
            avoidForceX *= 0.5f; // Giảm lực né trái nếu bên trái đông
            SDL_Log("Prefer right: leftCount=%d, rightCount=%d", leftCount, rightCount);
        }
        else if (rightCount > leftCount && avoidForceX > 0) {
            avoidForceX *= 0.5f; // Giảm lực né phải nếu bên phải đông
            SDL_Log("Prefer left: leftCount=%d, rightCount=%d", leftCount, rightCount);
        }

        // Giới hạn lực
        avoidForceX = std::max(-MAX_FORCE, std::min(MAX_FORCE, avoidForceX));
        // Làm mịn lực để tránh rung lắc
        smoothedForceX = smoothedForceX * DAMPING + avoidForceX * (1.0f - DAMPING);
        x += smoothedForceX * 0.2f; // Giảm hệ số để mượt hơn
        SDL_Log("Criminal moving to x=%f, smoothedForceX=%f", x, smoothedForceX);
    }
    else {
        smoothedForceX *= DAMPING; // Giảm lực khi không né
    }

    // Giới hạn X
    x = std::max(0, std::min(x, 800 - rect.w));
    rect.x = x;

    // Kiểm tra và xử lý va chạm thực tế
    for (auto civ : civilianCars) {
        if (!civ) continue;
        SDL_Rect civRect = civ->GetRect();
        if (SDL_HasIntersection(&rect, &civRect)) {
            SDL_Log("Collision with civilian at x=%d, y=%d", civRect.x, civRect.y);
            // Đẩy ra khỏi xe dân
            if (civRect.x > x) {
                x -= AVOID_SPEED * 2.0f; // Đẩy trái
            }
            else {
                x += AVOID_SPEED * 2.0f; // Đẩy phải
            }
            x = std::max(0, std::min(x, 800 - rect.w));
            rect.x = x;
        }
    }
}

void CriminalCar::TakeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    SDL_Log("[Damage] HP: %d (Damage: %d)", hp, dmg);
}

bool CriminalCar::IsDead() const {
    return hp <= 0;
}

void CriminalCar::Render(SDL_Renderer* renderer, int cameraY) {
    if (hp <= 0) return;
    Car::Render(renderer, cameraY);

    SDL_Rect hpBarBack = { rect.x, (rect.y - cameraY) - 10, rect.w, 5 };
    SDL_Rect hpBarFront = { rect.x, (rect.y - cameraY) - 10, static_cast<int>(rect.w * ((float)hp / maxHp)), 5 };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarBack);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarFront);
}