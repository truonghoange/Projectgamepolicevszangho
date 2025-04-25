#pragma once
#include "Car.h"
#include <vector>

class CriminalCar : public Car {
public:
    CriminalCar(int x, int y);
    void Update() override;

    // Dùng để cập nhật các xe dân để né
    void SetCivilianCars(const std::vector<Car*>& civilians);
    void SetY(int newY) {
        y = newY;
        rect.y = y;
    }
   
    void TakeDamage(int dmg);
    bool IsDead() const;
	int GetHp() const {
		return hp;
	}
	int GetMaxHp() const {
		return maxHp;
	}
	// Thêm hàm để lấy vị trí y
	int GetY() const {
		return y;
	}
	// Thêm hàm để lấy vị trí x
	int GetX() const {
		return x;
	}
	void Render(SDL_Renderer* renderer, int CameraY);
    void ToggleSmoke() { smokeOn = !smokeOn; } // Bật/tắt khói
    int GetExplosionFrame() const { return explosionFrame; }

private:
    std::vector<Car*> civilianCars;
	const int maxHp = 1000; // Giới hạn máu tối đa
    int hp = 1000;
    // Biến cho animation khói
    int currentFrame;       
    int frameWidth;         
    int frameHeight;       
    int totalFrames;        
    Uint32 lastFrameUpdate; 
    bool smokeOn;           
    bool smokeWhite;        
    // Biến cho hiệu ứng nổ
    bool isExploding;
    int explosionFrame;
    int explosionFrameWidth;
    int explosionFrameHeight;
    int explosionTotalFrames;
    Uint32 lastExplosionUpdate;
    SDL_Texture* explosionTexture;
    
};

