#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
class Car {
public:
    Car(int x, int y);
    virtual ~Car();
    virtual void Update() = 0;
    virtual void Render(SDL_Renderer* renderer, int cameraY);
    SDL_Rect GetRect() const;
    int GetY() const { return y; }
    void LoadTexture(SDL_Renderer* renderer, const std::string& texturePath);
    


protected:
    int x, y;
    int speed;
    SDL_Rect rect;
    SDL_Color color;
    SDL_Texture* texture; // Texture cho xe
    
};