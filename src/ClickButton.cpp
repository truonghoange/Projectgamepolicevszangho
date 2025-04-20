#include "ClickButton.h"

ClickButton::ClickButton(SDL_Renderer* renderer, const std::string& normalTexturePath, const std::string& hoverTexturePath, int x, int y, int w, int h)
    : renderer(renderer), normalTexture(nullptr), hoverTexture(nullptr), isHovered(false), isFlashing(false), flashStartTime(0), flashCount(0) {
    buttonRect = { x, y, w, h };

    normalTexture = IMG_LoadTexture(renderer, normalTexturePath.c_str());
    if (!normalTexture) {
        SDL_Log("Không thể tải normal texture cho button: %s", IMG_GetError());
    }
    
    hoverTexture = IMG_LoadTexture(renderer, hoverTexturePath.c_str());
    if (!hoverTexture) {
        SDL_Log("Không thể tải hover texture cho button: %s", IMG_GetError());
    }
}

ClickButton::~ClickButton() {
    if (normalTexture) SDL_DestroyTexture(normalTexture);
    if (hoverTexture) SDL_DestroyTexture(hoverTexture);
}

bool ClickButton::IsClicked(int mouseX, int mouseY) {
    SDL_Point mousePoint = { mouseX, mouseY };
    return SDL_PointInRect(&mousePoint, &buttonRect);
}

void ClickButton::StartFlashing() {
    isFlashing = true;
    flashStartTime = SDL_GetTicks();
    flashCount = 0;
}

void ClickButton::Update(int mouseX, int mouseY) {
    SDL_Point mousePoint = { mouseX, mouseY };
    isHovered = SDL_PointInRect(&mousePoint, &buttonRect);

    if (isFlashing) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - flashStartTime >= 200) { // Chuyển trạng thái mỗi 200ms
            flashCount++;
            flashStartTime = currentTime;
        }
        if (flashCount >= 6) { // Nhấp nháy 3 lần (6 trạng thái)
            isFlashing = false;
            flashCount = 0;
        }
    }
}

void ClickButton::Render() {
    bool flashState = false;
    if (isFlashing) {
        flashState = (flashCount % 2 == 0); // Đổi trạng thái: 0, 2, 4 là normal; 1, 3, 5 là hover
    }

    if (isFlashing) {
        SDL_RenderCopy(renderer, flashState ? normalTexture : hoverTexture, nullptr, &buttonRect);
    }
    else {
        SDL_RenderCopy(renderer, isHovered ? hoverTexture : normalTexture, nullptr, &buttonRect);
    }
}