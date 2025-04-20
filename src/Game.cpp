#include "Game.h"


SDL_Renderer* Game::renderer = nullptr;
int Game::cameraY = 0;

Game::Game() : window(nullptr), isRunning(false), showMessage(false), messageStartTime(0), font(nullptr), messageTexture(nullptr),
gameState(START_SCREEN), backgroundTexture(nullptr), instructionsTexture(nullptr), gameOverExplosionTexture(nullptr),
startButton(nullptr), instructionsButton(nullptr), exitButton(nullptr), replayButton(nullptr), countdown(nullptr) {
}

Game::~Game() {}

bool Game::Init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        return false;
    }
    if (TTF_Init() < 0) {
        SDL_Log("Không thể khởi tạo SDL_ttf: %s", TTF_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    font = TTF_OpenFont("assets/Roboto-VariableFont_wdth,wght.ttf", 18 );
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
    gameOverExplosionTexture = IMG_LoadTexture(renderer, "assets/over.png");
    if (!gameOverExplosionTexture) {
        SDL_Log("Không thể tải game over explosion texture: %s", IMG_GetError());
        return false;
    }
    // Khởi tạo các nút, bỏ nút Settings
    startButton = new ClickButton(renderer, "assets/start1.png", "assets/start2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 - 100, 200, 50);
    instructionsButton = new ClickButton(renderer, "assets/more1.png", "assets/more2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 - 20, 200, 50);
    exitButton = new ClickButton(renderer, "assets/exit1.png", "assets/exit2.png", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 60, 200, 50);
    replayButton = new ClickButton(renderer, "assets/continue1.png", "assets/continue2.png", (SCREEN_WIDTH - 200) / 2 , SCREEN_HEIGHT / 2 + 20, 200, 50);
    // Khởi tạo countdown
    countdown = new Countdown(renderer, font);
    if (!countdown) {
        SDL_Log("Không thể khởi tạo countdown");
        return false;
    }

    srand(static_cast<unsigned int>(time(NULL)));
    SDL_Log("Random seed initialized with time: %u", static_cast<unsigned int>(time(NULL)));

    background = new BackGround(renderer);

    lastCivilianSpawnTime = SDL_GetTicks();

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
    }

    cars.push_back(new CivilianCar(200, -100));
    cars.push_back(new CivilianCar(300, -300));
    cars.push_back(new CivilianCar(500, -500));
    cars.push_back(new CivilianCar(600, -700));

    CriminalCar* criminal = new CriminalCar(400, 100);
    cars.push_back(criminal);

    policeCar = new PoliceCar(400, 500);
    cars.push_back(policeCar);

    std::vector<Car*> civilians;
    for (auto car : cars) {
        if (dynamic_cast<CivilianCar*>(car)) {
            civilians.push_back(car);
        }
    }
    criminal->SetCivilianCars(civilians);

    cameraY = 0;
    lastCivilianSpawnTime = SDL_GetTicks();
    showMessage = false;
}

void Game::Run() {
    while (isRunning) {
        HandleEvents();
        Update();
        Render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::HandleEvents() {
    SDL_Event e;
    int mouseX, mouseY;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
            return;
        }
    }
    SDL_GetMouseState(&mouseX, &mouseY);
    if (gameState == START_SCREEN) {
        startButton->Update(mouseX, mouseY);
        instructionsButton->Update(mouseX, mouseY);
        exitButton->Update(mouseX, mouseY);
    }
    else if (gameState == GAME_OVER) {
        replayButton->Update(mouseX, mouseY);
        exitButton->Update(mouseX, mouseY);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (gameState == START_SCREEN) {
            if (startButton->IsClicked(mouseX, mouseY)) {
                startButton->StartFlashing();
                gameState = COUNTDOWN;
                ResetGame();
                countdown->Start(); // Bắt đầu countdown
                SDL_Log("Start Game clicked, entering countdown");
            }
            else if (instructionsButton->IsClicked(mouseX, mouseY)) {
                instructionsButton->StartFlashing();
                gameState = INSTRUCTIONS;
                SDL_Log("Instructions clicked");
            }
            else if (exitButton->IsClicked(mouseX, mouseY)) {
                exitButton->StartFlashing();
                isRunning = false;
                SDL_Log("Exit clicked");
            }
        }
        else if (gameState == INSTRUCTIONS) {
            gameState = START_SCREEN;
            SDL_Log("Returning to Start Screen");
        }
        else if (gameState == GAME_OVER) {
            if (replayButton->IsClicked(mouseX, mouseY)) {
                replayButton->StartFlashing();
                gameState = COUNTDOWN;
                ResetGame();
                countdown->Start(); // Bắt đầu countdown
                SDL_Log("Replay clicked, entering countdown");
            }
            else if (exitButton->IsClicked(mouseX, mouseY)) {
                exitButton->StartFlashing();
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
    int xPos = rand() % (800 - 50);
    int yOffset = -150 - (rand() % 100);
    int spawnY = cameraY + yOffset;

    // Kiểm tra chồng lấn
    SDL_Rect newRect = { xPos, spawnY, 70, 70 };
    for (auto car : cars) {
        SDL_Rect carRect = car->GetRect();
        if (SDL_HasIntersection(&newRect, &carRect)) {
            SDL_Log("Skipped spawn due to overlap at x=%d, y=%d", xPos, spawnY);
            return false;
        }
    }
    // create xe tren cartype
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
        }
        return; // Không cập nhật xe trong lúc countdown
    }
    if (gameState != PLAYING) return;

    if (showMessage) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - messageStartTime >= MESSAGE_DURATION) {
            gameState = GAME_OVER;
            return;
        }
        return;
    }

    SDL_Log("Update: cars=%zu, bullets=%zu", cars.size(), policeCar ? policeCar->GetBullets().size() : 0);

    // Tìm xe tội phạm
    CriminalCar* criminal = nullptr;
    for (auto car : cars) {
        criminal = dynamic_cast<CriminalCar*>(car);
        if (criminal) break;
    }

    // Cập nhật camera
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

    // Cập nhật danh sách xe dân cho xe tội phạm
    if (criminal) {
        std::vector<Car*> civilians;
        for (auto car : cars) {
            if (dynamic_cast<CivilianCar*>(car)) {
                civilians.push_back(car);
            }
        }
        criminal->SetCivilianCars(civilians);
    }

    // Kiểm tra va chạm giữa PoliceCar và CivilianCar
    if (policeCar) {
        SDL_Rect policeRect = policeCar->GetRect();
        for (auto car : cars) {
            if (auto civilian = dynamic_cast<CivilianCar*>(car)) {
                SDL_Rect civRect = civilian->GetRect();
                if (SDL_HasIntersection(&policeRect, &civRect)) {
                    SDL_Log("Game Over: Police car hit civilian at (%d,%d)", civRect.x, civRect.y);
                    // Thêm vụ nổ tại vị trí va chạm
                    int explosionX = policeRect.x + policeRect.w / 2;
                    int explosionY = policeRect.y + policeRect.h / 2;
                    explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                    SDL_Log("Created explosion on police-civilian collision at x=%d, y=%d", explosionX, explosionY);
                    showMessage = true;
                    gameMessage = "Game Over";
                    CreateMessageTexture(gameMessage, { 255, 0, 0, 255 });
                    messageStartTime = SDL_GetTicks();
                    return;
                }
            }
        }
    }

    // Kiểm tra khoảng cách: cảnh sát quá xa → thua
    if (policeCar && criminal) {
        int distance = policeCar->GetY() - criminal->GetY();
        if (distance > 500) {
            SDL_Log("Game Over: Police car too far behind");
            // Thêm vụ nổ tại vị trí xe cảnh sát
            int explosionX = policeCar->GetRect().x + policeCar->GetRect().w / 2;
            int explosionY = policeCar->GetRect().y + policeCar->GetRect().h / 2;
            explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
            SDL_Log("Created explosion on police car at x=%d, y=%d", explosionX, explosionY);
            showMessage = true;
            gameMessage = "Game Over, Distance > 500 you have been left behind by crime";
            CreateMessageTexture(gameMessage, { 255, 0, 0, 255 });
            messageStartTime = SDL_GetTicks();
            return;
        }
    }

    // Cập nhật tất cả xe
    for (auto car : cars) {
        if (car) car->Update();
    }

    // Cập nhật đạn
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

            // Kiểm tra va chạm với xe tội phạm
            SDL_Rect criminalRect = criminal->GetRect();
            if (SDL_HasIntersection(&bulletRect, &criminalRect)) {
                criminal->TakeDamage(20);
                SDL_Log("Bullet hit criminal, HP=%d", criminal->GetHp());
                // Thêm vụ nổ tại vị trí đạn trúng
                int explosionX = bulletRect.x;
                int explosionY = bulletRect.y;
                explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                SDL_Log("Created explosion at x=%d, y=%d", explosionX, explosionY);
                shouldDelete = true;
                if (criminal->IsDead()) {
                    SDL_Log("Criminal dead!");
                    // Thêm một vụ nổ nữa khi xe tội phạm bị tiêu diệt
                    explosionX = criminal->GetRect().x + criminal->GetRect().w / 2;
                    explosionY = criminal->GetRect().y + criminal->GetRect().h / 2;
                    explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                    SDL_Log("Created explosion on criminal death at x=%d, y=%d", explosionX, explosionY);
                    showMessage = true;
                    gameMessage = "Mission Complete";
                    CreateMessageTexture(gameMessage, { 0, 255, 0, 255 });
                    messageStartTime = SDL_GetTicks();
                    return;
                }
            }

            // Kiểm tra va chạm với xe dân
            for (auto car : cars) {
                if (auto civilian = dynamic_cast<CivilianCar*>(car)) {
                    SDL_Rect civRect = civilian->GetRect();
                    if (SDL_HasIntersection(&bulletRect, &civRect)) {
                        SDL_Log("Game Over: Bullet hit civilian at (%d,%d)", civRect.x, civRect.y);
                        // Thêm vụ nổ tại vị trí đạn trúng xe dân
                        int explosionX = bulletRect.x;
                        int explosionY = bulletRect.y;
                        explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                        SDL_Log("Created explosion on bullet-civilian collision at x=%d, y=%d", explosionX, explosionY);
                        showMessage = true;
                        gameMessage = "Game Over";
                        CreateMessageTexture(gameMessage, { 255, 0, 0, 255 });
                        messageStartTime = SDL_GetTicks();
                        shouldDelete = true;
                        return;
                    }
                }
            }

            // Xóa đạn nếu ra khỏi màn hình
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

    // Cập nhật các vụ nổ
    auto expIt = explosions.begin();
    while (expIt != explosions.end()) {
        ExplosionEffect* explosion = *expIt;
        explosion->Update();

        if (explosion->GetProtectedCurrentFrame() >= explosion->GetProtectedTotalFrames()) { // Kiểm tra xem vụ nổ đã kết thúc chưa
            SDL_Log("Explosion finished at x=%d, y=%d", explosion->GetX(), explosion->GetY());
            delete explosion;
            expIt = explosions.erase(expIt);
        }
        else {
            ++expIt;
        }
    }

    // Tạo xe dân
    static const int MAX_CIVILIANS = 30; // Tăng lên 30
    int civilianCount = 0;
    for (auto car : cars) {
        if (dynamic_cast<CivilianCar*>(car)) civilianCount++;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (civilianCount < MAX_CIVILIANS && currentTime - lastCivilianSpawnTime > 600) { // Giảm xuống 600ms
        int xPos = rand() % (600 - 50);
        // Spawn ở nhiều vị trí Y để đa dạng
        int yOffset = -150 - (rand() % 200); // Từ cameraY-150 đến cameraY-350
        cars.push_back(new CivilianCar(xPos, cameraY + yOffset));
        lastCivilianSpawnTime = currentTime;
        SDL_Log("Spawned civilian at x=%d, y=%d", xPos, cameraY + yOffset);
    }

    // Xóa xe dân ra khỏi màn hình
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
        exitButton->Render();
    }
    else if (gameState == INSTRUCTIONS) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        SDL_Rect instructionsRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, instructionsTexture, nullptr, &instructionsRect);
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
        SDL_RenderCopy(renderer, gameOverExplosionTexture, nullptr, nullptr);
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

    // Dọn dẹp explosions
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
    if (gameOverExplosionTexture) {
        SDL_DestroyTexture(gameOverExplosionTexture);
        gameOverExplosionTexture = nullptr;
    }

    delete startButton;
    startButton = nullptr;
    delete instructionsButton;
    instructionsButton = nullptr;
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