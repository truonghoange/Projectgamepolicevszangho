#pragma once


#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Countdown {
public:
    Countdown(SDL_Renderer* renderer, TTF_Font* font);
    ~Countdown();

    void Start(); // Bắt đầu countdown
    bool IsFinished() const; // Kiểm tra xem countdown đã kết thúc chưa
    void Update(); // Cập nhật trạng thái countdown
    void Render(); // Vẽ countdown lên màn hình

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* countdownFont;
    Uint32 startTime;
    bool isFinished;
    SDL_Texture* countdownTexture;
    SDL_Rect countdownRect;

    void CreateCountdownTexture(const std::string& text, SDL_Color color);
};

