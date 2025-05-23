﻿#pragma once
#include "Car.h"
#include "Bullet.h"
#include <vector>

class PoliceCar : public Car {
private:
    std::vector<Bullet*> bullets;
    bool canShoot;
    Uint32 lastShotTime;

    // Biến cho animation siren
    bool sirenOn;           // Trạng thái siren (bật/tắt)
    Uint32 lastSirenUpdate; // Thời gian cập nhật siren cuối cùng
    bool sirenRed;          
    int currentFrame;      
    int frameWidth;         
    int frameHeight;        
    int totalFrames;        
    // Biến cho hiệu ứng nổ
    bool isExploding;
    int explosionFrame;
    int explosionFrameWidth;
    int explosionFrameHeight;
    int explosionTotalFrames;
    Uint32 lastExplosionUpdate;
    SDL_Texture* explosionTexture;  
public:
    PoliceCar(int x, int y);
    ~PoliceCar();
    void Update() override;
    void HandleInput(const Uint8* keystates);
    void Render(SDL_Renderer* renderer, int cameraY) override;
    std::vector<Bullet*>& GetBullets();
    void Shoot();
    void ToggleSiren() { sirenOn = !sirenOn; }
    bool IsExploding() const { return isExploding; }
    int GetExplosionFrame() const { return explosionFrame; }
    void StartExplosion();
};