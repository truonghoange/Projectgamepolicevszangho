#include "Game.h"
#include "PoliceCar.h"
#include "CriminalCar.h"
#include "CivilianCar.h"
#include "Bullet.h"
#include"Car.h"
#include"BackGround.h"
#include"ExplosionEffect.h"


SDL_Renderer* Game::renderer = nullptr;
int Game::cameraY = 0;
Game::Game() : window(nullptr), isRunning(false) {}

Game::~Game() {}

bool Game::Init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;


    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    srand(static_cast<unsigned int>(time(NULL)));
    SDL_Log("Random seed initialized with time: %u", static_cast<unsigned int>(time(NULL)));

    background = new BackGround(renderer);

    lastCivilianSpawnTime = SDL_GetTicks();

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

    isRunning = true;
    return true;
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
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) isRunning = false;
    }
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (policeCar) {
        policeCar->HandleInput(keystates);
    }
}
bool Game::SpawnCar(const char* carType) {
    int xPos = rand() % (600 - 50);
    int yOffset = -150 - (rand() % 100);
    int spawnY = cameraY + yOffset;

    // Kiểm tra chồng lấn
    SDL_Rect newRect = { xPos, spawnY, 80, 80 };
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
void Game::Update() {
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
                    isRunning = false;
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
            isRunning = false;
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
                int explosionX = bulletRect.x; // Vị trí đạn
                int explosionY = bulletRect.y;
                explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                SDL_Log("Created explosion at x=%d, y=%d", explosionX, explosionY);
                shouldDelete = true;
                if (criminal->IsDead()) {
                    SDL_Log("Criminal dead!");
                    explosionX = criminal->GetRect().x + criminal->GetRect().w / 2;
                    explosionY = criminal->GetRect().y + criminal->GetRect().h / 2;
                    explosions.push_back(new ExplosionEffect(explosionX, explosionY, renderer));
                    SDL_Log("Created explosion on criminal death at x=%d, y=%d", explosionX, explosionY);
                    isRunning = false;
                    return;
                }
            }

            // Kiểm tra va chạm với xe dân
            for (auto car : cars) {
                if (auto civilian = dynamic_cast<CivilianCar*>(car)) {
                    SDL_Rect civRect = civilian->GetRect();
                    if (SDL_HasIntersection(&bulletRect, &civRect)) {
                        SDL_Log("Game Over: Bullet hit civilian at (%d,%d)", civRect.x, civRect.y);
                        shouldDelete = true;
                        isRunning = false;
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
    auto expIt = explosions.begin();
    while (expIt != explosions.end()) {
        ExplosionEffect* explosion = *expIt;
        explosion->Update();

        if (explosion->GetCurrentFrame() >= explosion->GetTotalFrames()) {// Kiểm tra xem vụ nổ đã kết thúc chưa
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
    background->Render(renderer);
    for (auto car : cars) car->Render(renderer, cameraY);
    for (auto explosion : explosions) explosion->Render(renderer, cameraY);
    SDL_RenderPresent(renderer);
}

void Game::Clean() {
    for (auto car : cars) delete car;
    cars.clear();
    for (auto explosion : explosions) delete explosion;
    explosions.clear();
    delete background;
    background = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();


}