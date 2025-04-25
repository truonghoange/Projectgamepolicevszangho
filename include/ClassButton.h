#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class CustomButton {
public:
    CustomButton(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int w, int h, SDL_Color normalColor, SDL_Color hoverColor);
    ~CustomButton();

    void Update(int mouseX, int mouseY);
    bool IsClicked(int mouseX, int mouseY);
    void Render();

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    std::string text;
    SDL_Rect rect;
    SDL_Color normalColor;
    SDL_Color hoverColor;
    SDL_Color currentColor;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
    bool isHovered;
};