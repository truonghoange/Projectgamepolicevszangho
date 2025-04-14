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

private:
    std::vector<Car*> civilianCars;
	const int maxHp = 1000; // Giới hạn máu tối đa
    int hp = 1000;
    // Biến cho animation khói
    int currentFrame;       // Frame hiện tại trong sprite sheet
    int frameWidth;         // Chiều rộng mỗi frame
    int frameHeight;        // Chiều cao mỗi frame
    int totalFrames;        // Tổng số frame (3: khói trắng, khói xám, không khói)
    Uint32 lastFrameUpdate; // Thời gian cập nhật frame cuối cùng
    bool smokeOn;           // Trạng thái khói (bật/tắt)
    bool smokeWhite;        // Trạng thái khói: trắng (true) hay xám (false)
    
};

