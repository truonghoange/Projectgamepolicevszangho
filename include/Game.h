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
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include "PoliceCar.h"
#include"Car.h"
#include"BackGround.h"


using namespace std;
class Game {
public:
    Game();
    ~Game();
    
    static const int SCREEN_WIDTH = 600;  
    static const int SCREEN_HEIGHT = 600; 
    static const int MAX_BACKGROUND_HEIGHT = 2400;
        // Other members of Game...
	static SDL_Renderer* renderer;
    static int cameraY; 
    bool Init(const char* title, int width, int height);
    void Run();
    void Clean();


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
};

