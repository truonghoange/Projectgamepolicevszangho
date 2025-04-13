#pragma once
#include "Car.h"

class CivilianCar : public Car {
public:
    CivilianCar(int x, int y);
    void Update() override;
    int GetY() const {
        return y;
    };
 
};
