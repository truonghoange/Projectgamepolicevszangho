#pragma once
#include "Car.h"
#include "Bullet.h"
#include <vector>


class PoliceCar : public Car {
public:
    PoliceCar(int x, int y);
    void Update() override;
    void HandleInput(const Uint8* keystates);
    void Render(SDL_Renderer* renderer, int cameraY) override;
	
	void Shoot(); // thêm hàm bắn đạn
    std::vector<Bullet*>& GetBullets(); // thêm getter
	void clean() {
		for (auto bullet : bullets) {
			delete bullet;
		}
		bullets.clear();
	} // thêm hàm xóa đạn
	void SetY(int newY) {
		y = newY;
		rect.y = y;
	} // thêm hàm set y
    int GetY() const {  
       return y;  
    } // Fix return type to match the function type
private:
    std::vector<Bullet*> bullets;
	bool canShoot = true; // biến kiểm tra có thể bắn hay không

};
