#include "CivilianCar.h"

CivilianCar::CivilianCar(int x, int y) : Car(x, y) {
    color = { 200, 200, 200, 255 }; // Màu xám (xe dân)
    speed = 2;
}

void CivilianCar::Update() {
    y += speed; // Xe dân ch?y t? trên xu?ng d??i (ng??c chi?u)
    rect.y = y;
}