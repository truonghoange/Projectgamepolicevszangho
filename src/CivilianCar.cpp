#include "CivilianCar.h"

CivilianCar::CivilianCar(int x, int y) : Car(x, y) {
    color = { 200, 200, 200, 255 }; // M�u x�m (xe d�n)
    speed = 2;
}

void CivilianCar::Update() {
    y += speed; // Xe d�n ch?y t? tr�n xu?ng d??i (ng??c chi?u)
    rect.y = y;
}