#include "Countdown.h"
#include "Game.h" // Thêm include để dùng SCREEN_WIDTH và SCREEN_HEIGHT

Countdown::Countdown(SDL_Renderer* renderer, TTF_Font* font)
    : renderer(renderer), font(font), countdownFont(nullptr), startTime(0), isFinished(false), countdownTexture(nullptr) {
    countdownRect = { 0, 0, 0, 0 };
    // Tạo font riêng cho countdown với kích thước lớn hơn (72)
    countdownFont = TTF_OpenFont("assets/Roboto-VariableFont_wdth,wght.ttf", 120);
    if (!countdownFont) {
        SDL_Log("Không thể mở font cho countdown: %s", TTF_GetError());
    }
}

Countdown::~Countdown() {
    if (countdownTexture) {
        SDL_DestroyTexture(countdownTexture);
        countdownTexture = nullptr;
    }
    if (countdownFont) {
        TTF_CloseFont(countdownFont);
        countdownFont = nullptr;
    }
}

void Countdown::Start() {
    startTime = SDL_GetTicks();
    isFinished = false;
    SDL_Log("Countdown started");
}

bool Countdown::IsFinished() const {
    return isFinished;
}

void Countdown::Update() {
    if (isFinished) return;

    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - startTime;

    if (elapsedTime >= 4000) { // Sau 4 giây (3, 2, 1, Start, mỗi số 1 giây)
        isFinished = true;
        SDL_Log("Countdown finished");
        return;
    }

    std::string countdownText;
    SDL_Color countdownColor = { 255, 204, 119, 34 }; // Màu cam cho "3", "2", "1"

    if (elapsedTime < 1000) {
        countdownText = "3";
    }
    else if (elapsedTime < 2000) {
        countdownText = "2";
    }
    else if (elapsedTime < 3000) {
        countdownText = "1";
    }
    else {
        countdownText = "Start!!!";
        countdownColor = { 255, 0, 0, 255 }; // Màu đỏ cho "Start", alpha = 255 để hiện rõ
    }

    CreateCountdownTexture(countdownText, countdownColor);
}

void Countdown::CreateCountdownTexture(const std::string& text, SDL_Color color) {
    if (countdownTexture) {
        SDL_DestroyTexture(countdownTexture);
        countdownTexture = nullptr;
    }

    // Sử dụng countdownFont (kích thước 72) thay vì font gốc
    SDL_Surface* surface = TTF_RenderText_Solid(countdownFont, text.c_str(), color);
    if (!surface) {
        SDL_Log("Không thể tạo surface cho countdown: %s", TTF_GetError());
        return;
    }

    countdownTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!countdownTexture) {
        SDL_Log("Không thể tạo texture cho countdown: %s", SDL_GetError());
        return;
    }

    SDL_QueryTexture(countdownTexture, nullptr, nullptr, &countdownRect.w, &countdownRect.h);
    countdownRect.x = (Game::SCREEN_WIDTH - countdownRect.w) / 2; // Sử dụng SCREEN_WIDTH từ Game.h (600)
    countdownRect.y = (Game::SCREEN_HEIGHT - countdownRect.h) / 2; // Sử dụng SCREEN_HEIGHT từ Game.h (600)
}

void Countdown::Render() {
    if (isFinished || !countdownTexture) return;
    SDL_RenderCopy(renderer, countdownTexture, nullptr, &countdownRect);
}