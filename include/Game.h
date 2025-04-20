#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <ctime>
#include <cstdlib>
#include <memory>
#include <thread>
#include <mutex>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "PoliceCar.h"
#include "Car.h"
#include "BackGround.h"
#include "CivilianCar.h"
#include "CriminalCar.h"
#include "Bullet.h"
#include "ExplosionEffect.h" 
#include "ClickButton.h"
#include "Countdown.h"
using namespace std;

class Game {
public:
    enum GameState {
        START_SCREEN,
        INSTRUCTIONS,
        PLAYING,
        GAME_OVER,
        COUNTDOWN
    };
    Game();
    ~Game();

    static const int SCREEN_WIDTH = 600;
    static const int SCREEN_HEIGHT = 600;
    static const int MAX_BACKGROUND_HEIGHT = 2400;
    static const int MESSAGE_DURATION = 2000;

    static SDL_Renderer* renderer;
    static int cameraY;
    static std::vector<Bullet*> bullets; // Thêm để quản lý đạn

    bool Init(const char* title, int width, int height);
    void ResetGame();
    void Run();
    void Clean();
    bool SpawnCar(const char* carType);
	void CreateMessageTexture(const std::string& message, SDL_Color color);
private:
    void HandleEvents();
    void Update();
    void Render();

    SDL_Window* window;
    bool isRunning;

    vector<Car*> cars;
    Uint32 lastCivilianSpawnTime = 0;
    PoliceCar* policeCar = nullptr;
    BackGround* background = nullptr;
    std::vector<ExplosionEffect*> explosions; // Thêm để quản lý hiệu ứng nổ
    bool showMessage;
    std::string gameMessage;
    Uint32 messageStartTime;
    TTF_Font* font;
    SDL_Texture* messageTexture;
    SDL_Rect messageRect;

    GameState gameState;
    SDL_Texture* backgroundTexture;
    SDL_Texture* instructionsTexture;
    SDL_Texture* gameOverExplosionTexture;

    ClickButton* startButton;
    ClickButton* instructionsButton;
    ClickButton* exitButton;
    ClickButton* replayButton;
    Countdown* countdown;
};