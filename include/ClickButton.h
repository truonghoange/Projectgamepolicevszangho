#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>

class ClickButton {
public:
    ClickButton(SDL_Renderer* renderer, const std::string& normalTexturePath, const std::string& hoverTexturePath, int x, int y, int w, int h);
    ~ClickButton();

    // Kiểm tra xem nút có được click không
    bool IsClicked(int mouseX, int mouseY);

    // Kích hoạt hiệu ứng nhấp nháy khi click
    void StartFlashing();

    // Cập nhật trạng thái hover và nhấp nháy
    void Update(int mouseX, int mouseY);

    // Vẽ nút
    void Render();

private:
    SDL_Renderer* renderer;
    SDL_Texture* normalTexture;
    SDL_Texture* hoverTexture;
    SDL_Rect buttonRect;

    bool isHovered;
    bool isFlashing;
    Uint32 flashStartTime;
    int flashCount;
};