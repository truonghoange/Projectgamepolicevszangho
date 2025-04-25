#include"ClassButton.h"	
CustomButton::CustomButton(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int w, int h, SDL_Color normalColor, SDL_Color hoverColor)
    : renderer(renderer), font(font), text(text), normalColor(normalColor), hoverColor(hoverColor), currentColor(normalColor), textTexture(nullptr), isHovered(false) {
    rect = { x, y, w, h };

    // Tạo texture cho văn bản
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255, 255 }); // Màu trắng cho chữ
    if (!surface) {
        SDL_Log("Không thể tạo surface cho nút: %s", TTF_GetError());
        return;
    }
    textTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!textTexture) {
        SDL_Log("Không thể tạo texture cho nút: %s", SDL_GetError());
        return;
    }

    SDL_QueryTexture(textTexture, nullptr, nullptr, &textRect.w, &textRect.h);
    textRect.x = x + (w - textRect.w) / 2; // Căn giữa văn bản
    textRect.y = y + (h - textRect.h) / 2;
}

CustomButton::~CustomButton() {
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
        textTexture = nullptr;
    }
}

void CustomButton::Update(int mouseX, int mouseY) {
    isHovered = (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h);
    currentColor = isHovered ? hoverColor : normalColor;
}

bool CustomButton::IsClicked(int mouseX, int mouseY) {
    return isHovered && SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT);
}

void CustomButton::Render() {
    // Vẽ nền nút
    SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    SDL_RenderFillRect(renderer, &rect);

    // Vẽ viền
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Viền trắng
    SDL_RenderDrawRect(renderer, &rect);

    // Vẽ văn bản
    if (textTexture) {
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    }
}