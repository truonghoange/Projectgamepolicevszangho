#include "Game.h"
// Các include khác giữ nguyên...

SDL_Renderer* Game::renderer = nullptr;
int Game::cameraY = 0;

Game::Game() : window(nullptr), isRunning(false), showMessage(false), messageStartTime(0), font(nullptr), messageTexture(nullptr),
gameState(START_SCREEN), backgroundTexture(nullptr), instructionsTexture(nullptr), winTexture(nullptr), gameOverTexture(nullptr),
map1BackgroundTexture(nullptr), map2BackgroundTexture(nullptr), map3BackgroundTexture(nullptr),
startButton(nullptr), instructionsButton(nullptr), mapButton(nullptr), easyButton(nullptr), normalButton(nullptr),
hardButton(nullptr), exitButton(nullptr), replayButton(nullptr), countdown(nullptr), selectedMap(1),
MAX_CIVILIANS(15), CIVILIAN_SPAWN_INTERVAL(1500), MAX_DISTANCE_TO_LOSE(500) {
}

Game::~Game() {}

bool Game::Init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("Không thể khởi tạo SDL: %s", SDL_GetError());
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        return false;
    }
    if (TTF_Init() < 0) {
        SDL_Log("Không thể khởi tạo SDL_ttf: %s", TTF_GetError());
        return false;
    }

    if (!SoundManager::GetInstance().Init()) {
        SDL_Log("Không thể khởi tạo SoundManager");
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    font = TTF_OpenFont("assets/Roboto-VariableFont_wdth,wght.ttf", 18);
    if (!font) {
        SDL_Log("Không thể mở font: %s", TTF_GetError());
        return false;
    }
    backgroundTexture = IMG_LoadTexture(renderer, "assets/posgame.png");
    if (!backgroundTexture) {
        SDL_Log("Không thể tải background texture: %s", IMG_GetError());
        return false;
    }
    instructionsTexture = IMG_LoadTexture(renderer, "assets/instruction.png");
    if (!instructionsTexture) {
        SDL_Log("Không thể tải instructions texture: %s", IMG_GetError());
        return false;
    }
    map1BackgroundTexture = IMG_LoadTexture(renderer, "map/road.png");
    if (!map1BackgroundTexture) {
        SDL_Log("Không thể tải map1 background texture: %s", IMG_GetError());
        return false;
    }
    map2BackgroundTexture = IMG_LoadTexture(renderer, "map/mapsamac.png");
    if (!map2BackgroundTexture) {
        SDL_Log("Không thể tải map2 background texture: %s", IMG_GetError());
        return false;
    }
    map3BackgroundTexture = IMG_LoadTexture(renderer, "map/mapmua.png");
    if (!map3BackgroundTexture) {
        SDL_Log("Không thể tải map3 background texture: %s", IMG_GetError());
        return false;
    }
    winTexture = IMG_LoadTexture(renderer, "assets/win.png");
    if (!winTexture) {
        SDL_Log("Không thể tải win background texture: %s", IMG_GetError());
        return false;
    }
    gameOverTexture = IMG_LoadTexture(renderer, "assets/over.png");
    if (!gameOverTexture) {
        SDL_Log("Không thể tải game over background texture: %s", IMG_GetError());
        return false;
    }

    startButton = new ClickButton(renderer, "assets/start1.png", "assets/start2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 - 100, 200, 50);
    instructionsButton = new ClickButton(renderer, "assets/more1.png", "assets/more2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 - 20, 200, 50);
    mapButton = new ClickButton(renderer, "assets/map1.png", "assets/map2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 140, 200, 50);
    easyButton = new CustomButton(renderer, font, "Easy", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 - 100, 200, 50, { 100, 200, 100, 255 }, { 150, 255, 150, 255 });
    normalButton = new CustomButton(renderer, font, "Normal", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 - 20, 200, 50, { 200, 200, 100, 255 }, { 255, 255, 150, 255 });
    hardButton = new CustomButton(renderer, font, "Hard", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 60, 200, 50, { 200, 100, 100, 255 }, { 255, 150, 150, 255 });
    exitButton = new ClickButton(renderer, "assets/exit1.png", "assets/exit2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 60, 200, 50);
    replayButton = new ClickButton(renderer, "assets/continue1.png", "assets/continue2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 20, 200, 50);

    countdown = new Countdown(renderer, font);
    if (!countdown) {
        SDL_Log("Không thể khởi tạo countdown");
        return false;
    }

    srand(static_cast<unsigned int>(time(NULL)));
    SDL_Log("Random seed initialized with time: %u", static_cast<unsigned int>(time(NULL)));

    background = nullptr;

    lastCivilianSpawnTime = SDL_GetTicks();

    SoundManager::GetInstance().PlayBackgroundMusic();

    isRunning = true;
    return true;
}

void Game::ResetGame() {
    for (auto car : cars) delete car;
    cars.clear();
    for (auto explosion : explosions) delete explosion;
    explosions.clear();
    if (policeCar) {
        auto& bullets = policeCar->GetBullets();
        for (auto bullet : bullets) delete bullet;
        bullets.clear();

        policeCar = nullptr;
    }
    if (background) {
        delete background;
        background = nullptr;
    }

    switch (selectedMap) {
    case 1:
        MAX_CIVILIANS = 20;
        CIVILIAN_SPAWN_INTERVAL = 1000;
        MAX_DISTANCE_TO_LOSE = 500;
        background = new BackGround(renderer, map1BackgroundTexture);
        break;
    case 2:
        MAX_CIVILIANS = 25;
        CIVILIAN_SPAWN_INTERVAL = 800;
        MAX_DISTANCE_TO_LOSE = 500;
        background = new BackGround(renderer, map2BackgroundTexture);
        break;
    case 3:
        MAX_CIVILIANS = 35;
        CIVILIAN_SPAWN_INTERVAL = 600;
        MAX_DISTANCE_TO_LOSE = 500;
        background = new BackGround(renderer, map3BackgroundTexture);
        break;
    default:
        MAX_CIVILIANS = 20;
        CIVILIAN_SPAWN_INTERVAL = 1000;
        MAX_DISTANCE_TO_LOSE = 500;
        background = new BackGround(renderer, map1BackgroundTexture);
        break;
    }

    const int CAR_WIDTH = 70;
    const int CAR_HEIGHT = 70;
    auto trySpawnCar = [&](int& x, int y) -> bool {
        SDL_Rect newRect = { x, y, CAR_WIDTH, CAR_HEIGHT };
        for (auto car : cars) {
            SDL_Rect carRect = car->GetRect();
            if (SDL_HasIntersection(&newRect, &carRect)) {
                return false;
            }
        }
        cars.push_back(new CivilianCar(x, y));
        return true;
        };

    int attempts = 0;
    while (cars.size() < 4 && attempts < 100) {
        int xPos = rand() % (SCREEN_WIDTH - CAR_WIDTH);
        int yPos = -100 - (rand() % 600);
        if (trySpawnCar(xPos, yPos)) {
            SDL_Log("Spawned CivilianCar at x=%d, y=%d", xPos, yPos);
        }
        attempts++;
    }

    CriminalCar* criminal = nullptr;
    attempts = 0;
    while (!criminal && attempts < 100) {
        int xPos = rand() % (SCREEN_WIDTH - CAR_WIDTH);
        int yPos = 100;
        SDL_Rect newRect = { xPos, yPos, CAR_WIDTH, CAR_HEIGHT };
        bool overlap = false;
        for (auto car : cars) {
            SDL_Rect carRect = car->GetRect();
            if (SDL_HasIntersection(&newRect, &carRect)) {
                overlap = true;
                break;
            }
        }
        if (!overlap) {
            criminal = new CriminalCar(xPos, yPos);

            switch (selectedMap) {
            case 1: {
                criminal->SetSpeed(4);
                break;
            }
            case 2: {
                criminal->SetSpeed(5);
                break;
            }
            case 3: {
                criminal->SetSpeed(6);
                break;
            }
            default: {
                criminal->SetSpeed(3);
                break;
            }
            }
            cars.push_back(criminal);
            SDL_Log("Spawned CriminalCar at x=%d, y=%d", xPos, yPos);
        }
        attempts++;
    }

    policeCar = nullptr;
    attempts = 0;
    while (!policeCar && attempts < 100) {
        int xPos = rand() % (SCREEN_WIDTH - CAR_WIDTH);
        int yPos = 500;
        SDL_Rect newRect = { xPos, yPos, CAR_WIDTH, CAR_HEIGHT };
        bool overlap = false;
        for (auto car : cars) {
            SDL_Rect carRect = car->GetRect();
            if (SDL_HasIntersection(&newRect, &carRect)) {
                overlap = true;
                break;
            }
        }
        if (!overlap) {
            policeCar = new PoliceCar(xPos, yPos);
            switch (selectedMap) {
            case 1:
                policeCar->SetSpeed(5);
                break;
            case 2:
                policeCar->SetSpeed(6);
                break;
            case 3:
                policeCar->SetSpeed(7);
                break;
            default:
                policeCar->SetSpeed(5);
                break;
            }
            cars.push_back(policeCar);
            SDL_Log("Spawned PoliceCar at x=%d, y=%d", xPos, yPos);
        }
        attempts++;
    }

    std::vector<Car*> civilians;
    for (auto car : cars) {
        if (dynamic_cast<CivilianCar*>(car)) {
            civilians.push_back(car);
        }
    }
    if (criminal) {
        criminal->SetCivilianCars(civilians);
    }

    cameraY = 0;
    lastCivilianSpawnTime = SDL_GetTicks();
    showMessage = false;
}

void Game::Run() {
    while (isRunning) {
        HandleEvents();
        Update();
        Render();
        SDL_Delay(16);
    }
}

void Game::HandleEvents() {
    SDL_Event e;
    int mouseX, mouseY;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
            SoundManager::GetInstance().StopBackgroundMusic(); // Dừng nhạc nền khi thoát game
            return;
        }
    }
    SDL_GetMouseState(&mouseX, &mouseY);
    if (gameState == START_SCREEN) {
        startButton->Update(mouseX, mouseY);
        instructionsButton->Update(mouseX, mouseY);
        mapButton->Update(mouseX, mouseY);
        exitButton->Update(mouseX, mouseY);
    }
    else if (gameState == MAP_SELECTION) {
        easyButton->Update(mouseX, mouseY);
        normalButton->Update(mouseX, mouseY);
        hardButton->Update(mouseX, mouseY);
    }
    else if (gameState == GAME_OVER || gameState == WIN) {
        replayButton->Update(mouseX, mouseY);
        exitButton->Update(mouseX, mouseY);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (gameState == START_SCREEN) {
            if (startButton->IsClicked(mouseX, mouseY)) {
                startButton->StartFlashing();
                gameState = COUNTDOWN;
                ResetGame();
                countdown->Start();
                SDL_Log("Start Game clicked, entering countdown");
            }
            else if (instructionsButton->IsClicked(mouseX, mouseY)) {
                instructionsButton->StartFlashing();
                gameState = INSTRUCTIONS;
                SDL_Log("Instructions clicked");
            }
            else if (mapButton->IsClicked(mouseX, mouseY)) {
                mapButton->StartFlashing();
                gameState = MAP_SELECTION;
                SDL_Log("Map selection clicked");
            }
            else if (exitButton->IsClicked(mouseX, mouseY)) {
                exitButton->StartFlashing();
                SoundManager::GetInstance().StopBackgroundMusic(); // Dừng nhạc nền khi thoát
                isRunning = false;
                SDL_Log("Exit clicked");
            }
        }
        else if (gameState == INSTRUCTIONS) {
            gameState = START_SCREEN;
            SDL_Log("Returning to Start Screen");
        }
        else if (gameState == MAP_SELECTION) {
            if (easyButton->IsClicked(mouseX, mouseY)) {
                selectedMap = 1;
                gameState = START_SCREEN;
                SDL_Log("Selected Easy");
            }
            else if (normalButton->IsClicked(mouseX, mouseY)) {
                selectedMap = 2;
                gameState = START_SCREEN;
                SDL_Log("Selected Normal");
            }
            else if (hardButton->IsClicked(mouseX, mouseY)) {
                selectedMap = 3;
                gameState = START_SCREEN;
                SDL_Log("Selected Hard");
            }
        }
        else if (gameState == GAME_OVER) {
            if (replayButton->IsClicked(mouseX, mouseY)) {
                replayButton->StartFlashing();
                SoundManager::GetInstance().StopBackgroundMusic(); // Dừng nhạc nền trước khi replay
                gameState = COUNTDOWN;
                ResetGame();
                countdown->Start();
                SDL_Log("Replay clicked, entering countdown");
            }
            else if (exitButton->IsClicked(mouseX, mouseY)) {
                exitButton->StartFlashing();
                SoundManager::GetInstance().StopBackgroundMusic(); // Dừng nhạc nền khi thoát
                isRunning = false;
                SDL_Log("Exit clicked");
            }
        }
        else if (gameState == WIN) {
            if (replayButton->IsClicked(mouseX, mouseY)) {
                replayButton->StartFlashing();
                SoundManager::GetInstance().StopBackgroundMusic(); // Dừng nhạc nền trước khi replay
                selectedMap++;
                if (selectedMap > 3) {
                    selectedMap = 1;
                    SDL_Log("Completed all maps, looping back to Map 1");
                }
                else {
                    SDL_Log("Advancing to Map %d", selectedMap);
                }
                gameState = COUNTDOWN;
                ResetGame();
                countdown->Start();
                SDL_Log("Continue clicked, entering countdown for Map %d", selectedMap);
            }
            else if (exitButton->IsClicked(mouseX, mouseY)) {
                exitButton->StartFlashing();
                SoundManager::GetInstance().StopBackgroundMusic(); // Dừng nhạc nền khi thoát
                isRunning = false;
                SDL_Log("Exit clicked");
            }
        }
    }
    if (gameState == PLAYING) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (policeCar) {
            policeCar->HandleInput(keystates);
        }
    }
}

bool Game::SpawnCar(const char* carType) {
    const int CAR_WIDTH = 70;
    const int CAR_HEIGHT = 70;
    int xPos = rand() % (SCREEN_WIDTH - CAR_WIDTH);
    int yOffset = -150 - (rand() % 100);
    int spawnY = cameraY + yOffset;

    SDL_Rect newRect = { xPos, spawnY, CAR_WIDTH, CAR_HEIGHT };
    for (auto car : cars) {
        SDL_Rect carRect = car->GetRect();
        if (SDL_HasIntersection(&newRect, &carRect)) {
            SDL_Log("Skipped spawn due to overlap at x=%d, y=%d", xPos, spawnY);
            return false;
        }
    }

    if (strcmp(carType, "Civilian") == 0) {
        cars.push_back(new CivilianCar(xPos, spawnY));
        SDL_Log("Spawned %s at x=%d, y=%d", carType, xPos, spawnY);
        return true;
    }
    else if (strcmp(carType, "Criminal") == 0) {
        cars.push_back(new CriminalCar(xPos, spawnY));
        SDL_Log("Spawned %s at x=%d, y=%d", carType, xPos, spawnY);
        return true;
    }
    else {
        SDL_Log("Unknown car type: %s", carType);
        return false;
    }
}

void Game::CreateMessageTexture(const std::string& message, SDL_Color color) {
    if (messageTexture) {
        SDL_DestroyTexture(messageTexture);
        messageTexture = nullptr;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        SDL_Log("Không thể tạo surface cho thông báo: %s", TTF_GetError());
        return;
    }

    messageTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!messageTexture) {
        SDL_Log("Không thể tạo texture cho thông báo: %s", SDL_GetError());
        return;
    }

    SDL_QueryTexture(messageTexture, nullptr, nullptr, &messageRect.w, &messageRect.h);
    messageRect.x = (Game::SCREEN_WIDTH - messageRect.w) / 2;
    messageRect.y = (Game::SCREEN_HEIGHT - messageRect.h) / 2;
}

void Game::Update() {
    if (gameState == COUNTDOWN) {
        countdown->Update();
        if (countdown->IsFinished()) {
            gameState = PLAYING;
            SDL_Log("Countdown finished, starting game");
            SoundManager::GetInstance().PlayEngineRev();
            SoundManager::GetInstance().StopBackgroundMusic();
        }
        return;
    }
    if (gameState != PLAYING) {
        if (gameState == START_SCREEN) {
            SoundManager::GetInstance().PlayBackgroundMusic();
        }
        static bool playedEndSound = false;
        if ((gameState == WIN || gameState == GAME_OVER) && !playedEndSound) {
            SoundManager::GetInstance().PlayEngineRev();
            SoundManager::GetInstance().PlayBackgroundMusic(); // Phát nhạc nền khi WIN hoặc GAME_OVER
            playedEndSound = true;
        }
        else if (gameState != WIN && gameState != GAME_OVER) {
            playedEndSound = false;
        }
        return;
    }

    if (showMessage) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - messageStartTime >= MESSAGE_DURATION) {
            if (gameMessage == "Mission Complete") {
                gameState = WIN;
            }
            else {
                gameState = GAME_OVER;
            }
            return;
        }
        return;
    }

    SDL_Log("Update: cars=%zu, bullets=%zu", cars.size(), policeCar ? policeCar->GetBullets().size() : 0);

    CriminalCar* criminal = nullptr;
    for (auto car : cars) {
        criminal = dynamic_cast<CriminalCar*>(car);
        if (criminal) break;
    }

    if (criminal) {
        cameraY = criminal->GetY() - (Game::SCREEN_HEIGHT / 5);
        if (cameraY > Game::MAX_BACKGROUND_HEIGHT - Game::SCREEN_HEIGHT) {
            cameraY = Game::MAX_BACKGROUND_HEIGHT - Game::SCREEN_HEIGHT;
        }
        SDL_Log("cameraY=%d, criminalY=%d", cameraY, criminal->GetY());
    }
    else {
        SDL_Log("Warning: Criminal car not found!");
    }

    if (criminal) {
        std::vector<Car*> civilians;
        for (auto car : cars) {
            if (dynamic_cast<CivilianCar*>(car)) {
                civilians.push_back(car);
            }
        }
        criminal->SetCivilianCars(civilians);
    }

    if (policeCar) {
        SDL_Rect policeRect = policeCar->GetRect();
        for (auto car : cars) {
            if (auto civilian = dynamic_cast<CivilianCar*>(car)) {
                SDL_Rect civRect = civilian->GetRect();
                if (SDL_HasIntersection(&policeRect, &civRect)) {
                    SDL_Log("Game Over: Police car hit civilian at (%d,%d)", civRect.x, civRect.y);
                    int explosionX = policeRect.x + policeRect.w / 2;
                    int explosionY = policeRect.y + policeRect.h / 2;
                    explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                    SDL_Log("Created explosion on police-civilian collision at x=%d, y=%d", explosionX, explosionY);
                    SoundManager::GetInstance().PlayExplosion();
                    showMessage = true;
                    gameMessage = "Game Over, you were var civilianCar";
                    CreateMessageTexture(gameMessage, { 255, 0, 0, 255 });
                    messageStartTime = SDL_GetTicks();
                    return;
                }
            }
        }
    }

    if (policeCar && criminal) {
        int distance = policeCar->GetY() - criminal->GetY();
        if (distance > MAX_DISTANCE_TO_LOSE) {
            SDL_Log("Game Over: Police car too far behind");
            int explosionX = policeCar->GetRect().x + policeCar->GetRect().w / 2;
            int explosionY = policeCar->GetRect().y + policeCar->GetRect().h / 2;
            explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
            SDL_Log("Created explosion on police car at x=%d, y=%d", explosionX, explosionY);
            SoundManager::GetInstance().PlayExplosion();
            showMessage = true;
            gameMessage = "Game Over, Distance > %d you have been left behind by crime";
            CreateMessageTexture(gameMessage, { 255, 0, 0, 255 });
            messageStartTime = SDL_GetTicks();
            return;
        }
    }

    for (auto car : cars) {
        if (car) car->Update();
    }

    if (policeCar && criminal) {
        auto& bullets = policeCar->GetBullets();
        auto it = bullets.begin();
        while (it != bullets.end()) {
            Bullet* bullet = *it;
            if (!bullet) {
                it = bullets.erase(it);
                continue;
            }
            bullet->Update();

            SDL_Rect bulletRect = bullet->GetRect();
            bool shouldDelete = false;

            SDL_Rect criminalRect = criminal->GetRect();
            if (SDL_HasIntersection(&bulletRect, &criminalRect)) {
                criminal->TakeDamage(20);
                SDL_Log("Bullet hit criminal, HP=%d", criminal->GetHp());
                int explosionX = bulletRect.x;
                int explosionY = bulletRect.y;
                explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                SDL_Log("Created explosion at x=%d, y=%d", explosionX, explosionY);
                SoundManager::GetInstance().PlayExplosion();
                shouldDelete = true;
                if (criminal->IsDead()) {
                    SDL_Log("Criminal dead!");
                    explosionX = criminal->GetRect().x + criminal->GetRect().w / 2;
                    explosionY = criminal->GetRect().y + criminal->GetRect().h / 2;
                    explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                    SDL_Log("Created explosion on criminal death at x=%d, y=%d", explosionX, explosionY);
                    SoundManager::GetInstance().PlayExplosion();
                    showMessage = true;
                    gameMessage = "Mission Complete";
                    CreateMessageTexture(gameMessage, { 0, 255, 0, 255 });
                    messageStartTime = SDL_GetTicks();
                    return;
                }
            }

            for (auto car : cars) {
                if (auto civilian = dynamic_cast<CivilianCar*>(car)) {
                    SDL_Rect civRect = civilian->GetRect();
                    if (SDL_HasIntersection(&bulletRect, &civRect)) {
                        SDL_Log("Game Over: Bullet hit civilian at (%d,%d)", civRect.x, civRect.y);
                        int explosionX = bulletRect.x;
                        int explosionY = bulletRect.y;
                        explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                        SDL_Log("Created explosion on bullet-civilian collision at x=%d, y=%d", explosionX, explosionY);
                        SoundManager::GetInstance().PlayExplosion();
                        showMessage = true;
                        gameMessage = "Game Over, you were hit civilianCar";
                        CreateMessageTexture(gameMessage, { 255, 0, 0, 255 });
                        messageStartTime = SDL_GetTicks();
                        shouldDelete = true;
                        return;
                    }
                }
            }

            int bulletScreenY = bullet->GetY() - cameraY;
            if (bulletScreenY < -100 || bulletScreenY > Game::SCREEN_HEIGHT + 100) {
                SDL_Log("Bullet deleted: absoluteY=%d, screenY=%d, cameraY=%d", bullet->GetY(), bulletScreenY, cameraY);
                shouldDelete = true;
            }

            if (shouldDelete) {
                delete bullet;
                it = bullets.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    auto expIt = explosions.begin();
    while (expIt != explosions.end()) {
        ExplosionEffect* explosion = *expIt;
        explosion->Update();

        if (explosion->GetProtectedCurrentFrame() >= explosion->GetProtectedTotalFrames()) {
            SDL_Log("Explosion finished at x=%d, y=%d", explosion->GetX(), explosion->GetY());
            delete explosion;
            expIt = explosions.erase(expIt);
        }
        else {
            ++expIt;
        }
    }

    int civilianCount = 0;
    for (auto car : cars) {
        if (dynamic_cast<CivilianCar*>(car)) civilianCount++;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (civilianCount < MAX_CIVILIANS && currentTime - lastCivilianSpawnTime > CIVILIAN_SPAWN_INTERVAL) {
        int xPos = rand() % (SCREEN_WIDTH - 70);
        int yOffset = -150 - (rand() % 200);
        if (SpawnCar("Civilian")) {
            lastCivilianSpawnTime = currentTime;
        }
    }

    auto carIt = cars.begin();
    while (carIt != cars.end()) {
        if (auto civilian = dynamic_cast<CivilianCar*>(*carIt)) {
            int civScreenY = civilian->GetY() - cameraY;
            if (civScreenY > Game::SCREEN_HEIGHT + 100) {
                SDL_Log("Deleting civilian at y=%d", civilian->GetY());
                delete civilian;
                carIt = cars.erase(carIt);
                continue;
            }
        }
        ++carIt;
    }

    background->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameState == START_SCREEN) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        startButton->Render();
        instructionsButton->Render();
        mapButton->Render();
        exitButton->Render();
    }
    else if (gameState == INSTRUCTIONS) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        SDL_Rect instructionsRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, instructionsTexture, nullptr, &instructionsRect);
    }
    else if (gameState == MAP_SELECTION) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        easyButton->Render();
        normalButton->Render();
        hardButton->Render();
    }
    else if (gameState == COUNTDOWN) {
        background->Render(renderer);
        for (auto car : cars) car->Render(renderer, cameraY);
        countdown->Render();
    }
    else if (gameState == PLAYING) {
        background->Render(renderer);
        for (auto car : cars) car->Render(renderer, cameraY);
        for (auto explosion : explosions) explosion->Render(renderer, cameraY);
        if (showMessage && messageTexture) {
            SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
        }
    }
    else if (gameState == GAME_OVER) {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, nullptr);
        if (messageTexture) {
            SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
        }
        replayButton->Render();
        exitButton->Render();
    }
    else if (gameState == WIN) {
        SDL_RenderCopy(renderer, winTexture, nullptr, nullptr);
        if (messageTexture) {
            SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
        }
        replayButton->Render();
        exitButton->Render();
    }

    SDL_RenderPresent(renderer);
}

void Game::Clean() {
    for (auto car : cars) delete car;
    cars.clear();

    for (auto explosion : explosions) delete explosion;
    explosions.clear();

    if (messageTexture) {
        SDL_DestroyTexture(messageTexture);
        messageTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (instructionsTexture) {
        SDL_DestroyTexture(instructionsTexture);
        instructionsTexture = nullptr;
    }
    if (map1BackgroundTexture) {
        SDL_DestroyTexture(map1BackgroundTexture);
        map1BackgroundTexture = nullptr;
    }
    if (map2BackgroundTexture) {
        SDL_DestroyTexture(map2BackgroundTexture);
        map2BackgroundTexture = nullptr;
    }
    if (map3BackgroundTexture) {
        SDL_DestroyTexture(map3BackgroundTexture);
        map3BackgroundTexture = nullptr;
    }
    if (winTexture) {
        SDL_DestroyTexture(winTexture);
        winTexture = nullptr;
    }
    if (gameOverTexture) {
        SDL_DestroyTexture(gameOverTexture);
        gameOverTexture = nullptr;
    }

    SoundManager::GetInstance().Clean();

    delete startButton;
    startButton = nullptr;
    delete instructionsButton;
    instructionsButton = nullptr;
    delete mapButton;
    mapButton = nullptr;
    delete easyButton;
    easyButton = nullptr;
    delete normalButton;
    normalButton = nullptr;
    delete hardButton;
    hardButton = nullptr;
    delete exitButton;
    exitButton = nullptr;
    delete replayButton;
    replayButton = nullptr;
    delete countdown;
    countdown = nullptr;

    delete background;
    background = nullptr;

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}