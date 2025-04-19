#pragma once
#include "Car.h"

class CivilianCar : public Car {
private:
	int carType; // xe
    bool isExploding;
    int explosionFrame;
    int explosionFrameWidth;
    int explosionFrameHeight;
    int explosionTotalFrames;
    Uint32 lastExplosionUpdate;
    SDL_Texture* explosionTexture;
public:
    CivilianCar(int x, int y);
    ~CivilianCar();

    void Update() override;
    void StartExplosion();
    bool IsExploding() const { return isExploding; }
    int GetExplosionFrame() const { return explosionFrame; }
    int GetY() const {
        return y;
    };
	SDL_Rect GetRect() const override;
	void Render(SDL_Renderer* renderer, int cameraY) override;
 
};
