#pragma once
#include "Car.h"

class CivilianCar : public Car {
private:
	int carType; // xe
public:
    CivilianCar(int x, int y);
    void Update() override;
    int GetY() const {
        return y;
    };
	SDL_Rect GetRect() const override;
	void Render(SDL_Renderer* renderer, int cameraY) override;
 
};
