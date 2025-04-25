#include "CriminalCar.h"
#include "Game.h"
#include <algorithm>
#include <cmath>

CriminalCar::CriminalCar(int x, int y) : Car(x, y), hp(1000), maxHp(1000), currentFrame(0), frameWidth(50), frameHeight(70), totalFrames(3),
lastFrameUpdate(0), smokeOn(true), smokeWhite(true),
isExploding(false), explosionFrame(0), explosionFrameWidth(38), explosionFrameHeight(32),
explosionTotalFrames(5), lastExplosionUpdate(0), explosionTexture(nullptr)
{
    color = { 255, 0, 0, 255 };
    
    LoadTexture(Game::renderer, "assets/CriminalCar.png"); // Load sprite sheet
    explosionTexture = IMG_LoadTexture(Game::renderer, "assets/explosion.png");
    if (!explosionTexture) {
        SDL_Log("Failed to load explosion texture for criminal: %s", IMG_GetError());
    }
    rect.w = 50;
    rect.h = 70;
}

void CriminalCar::SetCivilianCars(const std::vector<Car*>& civilians) {
    civilianCars = civilians;
}
void CriminalCar::Update() {
    if (isExploding) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastExplosionUpdate >= 500) { // Chuyển frame 
            explosionFrame++;
            lastExplosionUpdate = currentTime;
        }
        return;
    }

    if (hp <= 0) return;

    // Di chuyển lên trên
    y -= speed;
    rect.y = y;
    SDL_Log("Criminal y=%d, speed=%d", y, speed);

    // Logic né tránh xe dân
    static float smoothedForceX = 0.0f;
    float avoidForceX = 0.0f;
    bool shouldAvoid = false;
    const float AVOID_RANGE_Y = 400.0f;
    const float AVOID_RANGE_X = 150.0f;
    const float AVOID_SPEED = speed * 3.0f;
    const float CLOSE_RANGE = 150.0f;
    const float DAMPING = 0.7f;
    const float MAX_FORCE = 10.0f;

    int leftCount = 0, rightCount = 0;

    for (auto civ : civilianCars) {
        if (!civ) continue;
        int civX = civ->GetRect().x;
        int civY = civ->GetRect().y;
        int distanceY = y - civY;
        int distanceX = x - civX;

        if (distanceY > 0 && distanceY < AVOID_RANGE_Y) {
            if (std::abs(distanceX) < AVOID_RANGE_X) {
                float weight = (1.0f - (distanceY / AVOID_RANGE_Y)) * (1.0f - (std::abs(distanceX) / AVOID_RANGE_X));
                if (distanceY < CLOSE_RANGE) weight *= 3.0f;
                SDL_Log("Criminal avoiding civilian at x=%d, y=%d, distanceY=%d, distanceX=%d, weight=%f", civX, civY, distanceY, distanceX, weight);
                shouldAvoid = true;
                float force = AVOID_SPEED * weight * (distanceX > 0 ? -1.0f : 1.0f);
                avoidForceX += force;

                if (distanceX > 0) leftCount++;
                else rightCount++;
            }
        }
    }

    if (shouldAvoid) {
        if (leftCount > rightCount && avoidForceX < 0) {
            avoidForceX *= 0.5f;
            SDL_Log("Prefer right: leftCount=%d, rightCount=%d", leftCount, rightCount);
        }
        else if (rightCount > leftCount && avoidForceX > 0) {
            avoidForceX *= 0.5f;
            SDL_Log("Prefer left: leftCount=%d, rightCount=%d", leftCount, rightCount);
        }

        avoidForceX = std::max(-MAX_FORCE, std::min(MAX_FORCE, avoidForceX));
        smoothedForceX = smoothedForceX * DAMPING + avoidForceX * (1.0f - DAMPING);
        x += smoothedForceX * 0.2f;
        SDL_Log("Criminal moving to x=%f, smoothedForceX=%f", x, smoothedForceX);
    }
    else {
        smoothedForceX *= DAMPING;
    }

    x = std::max(0, std::min(x, 600 - rect.w));
    rect.x = x;

    // Xử lý va chạm với xe dân (chỉ điều chỉnh x, không ảnh hưởng đến y)
    for (auto civ : civilianCars) {
        if (!civ) continue;
        SDL_Rect civRect = civ->GetRect();
        if (SDL_HasIntersection(&rect, &civRect)) {
            SDL_Log("Collision with civilian at x=%d, y=%d", civRect.x, civRect.y);
            if (civRect.x > x) {
                x -= AVOID_SPEED * 2.0f;
            }
            else {
                x += AVOID_SPEED * 2.0f;
            }
            x = std::max(0, std::min(x, 600 - rect.w));
            rect.x = x;
        }
    }
}

void CriminalCar::TakeDamage(int dmg) {
    if (hp <= 0) return;
    hp -= dmg;
    if (hp <= 0) {
        hp = 0;
        isExploding = true;
        lastExplosionUpdate = SDL_GetTicks();
        SDL_Log("Criminal car started exploding");
    }
    SDL_Log("[Damage] HP: %d (Damage: %d)", hp, dmg);
}

bool CriminalCar::IsDead() const {
    return hp <= 0;
}

void CriminalCar::Render(SDL_Renderer* renderer, int cameraY) {
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

    if (hp <= 0) return;

    // Vẽ xe với frame hiện tại
    SDL_Rect drawRect = rect;
    drawRect.y -= cameraY;
    if (smokeOn) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFrameUpdate >= 500) {
            smokeWhite = !smokeWhite;
            currentFrame = smokeWhite ? 0 : 1;
            lastFrameUpdate = currentTime;
        }
    }
    else {
        currentFrame = 2;
    }

    if (texture) {
        SDL_Rect srcRect = { currentFrame * frameWidth, 0, frameWidth, frameHeight };
        SDL_RenderCopy(renderer, texture, &srcRect, &drawRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &drawRect);
    }

    // Vẽ thanh HP
    SDL_Rect hpBarBack = { rect.x, (rect.y - cameraY) - 10, rect.w, 5 };
    SDL_Rect hpBarFront = { rect.x, (rect.y - cameraY) - 10, static_cast<int>(rect.w * ((float)hp / maxHp)), 5 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarBack);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarFront);
}