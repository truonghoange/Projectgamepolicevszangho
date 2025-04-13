#include "Game.h"
#include "PoliceCar.h"
#include "CriminalCar.h"
#include "CivilianCar.h"
#include "Bullet.h"
#include"Car.h"
#include"BackGround.h"


SDL_Renderer* Game::renderer = nullptr;
int Game::cameraY = 0;
Game::Game() : window(nullptr),  isRunning(false) {}

Game::~Game() {}    

bool Game::Init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    // ⚠️ Thêm đoạn này:
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

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

void Game::Update() {
    // Tìm xe tội phạm
    CriminalCar* criminal = nullptr;
    for (auto car : cars) {
        criminal = dynamic_cast<CriminalCar*>(car);
        if (criminal) break;
    }

    // Cập nhật camera theo vị trí Y của xe tội phạm
    if (criminal) {
        // Đặt camera để xe địch ở giữa màn hình

        cameraY = criminal->GetY() - (Game::SCREEN_HEIGHT / 2);
        SDL_Log("cameraY=%d, criminalY=%d", cameraY, criminal->GetY());

        
    }

    // Kiểm tra khoảng cách: nếu cảnh sát bị bỏ quá xa → thua
    if (policeCar && criminal) {
        int distance = policeCar->GetY() - criminal->GetY();
        if (distance > 500) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                "Game Over", "Police car left too far behind!", nullptr);
            isRunning = false;
        }
    }

    // Cập nhật tất cả xe (chúng tự xử lý logic của mình)
    for (auto car : cars) {
        car->Update();
    }

    // Cập nhật đạn
    if (policeCar && criminal) {
        auto& bullets = policeCar->GetBullets();
        auto it = bullets.begin();
        while (it != bullets.end()) {
            Bullet* bullet = *it;
            bullet->Update();

            // Lấy vị trí tuyệt đối của đạn và xe tội phạm
            SDL_Rect bulletRect = bullet->GetRect();
            SDL_Rect criminalRect = criminal->GetRect();

            bool shouldDelete = false;

            // Kiểm tra va chạm dựa trên vị trí tuyệt đối
            if (SDL_HasIntersection(&bulletRect, &criminalRect)) {
                criminal->TakeDamage(20);
                shouldDelete = true;
            }

            // Xóa đạn nếu ra khỏi khu vực hiển thị (dựa trên tọa độ tương đối)
            int bulletScreenY = bullet->GetY() - cameraY; // Tọa độ Y trên màn hình
           
            if (bulletScreenY < -100 || bulletScreenY > Game::SCREEN_HEIGHT + 100) {
                SDL_Log("Bullet deleted at Y=%d, cameraY=%d, bulletScreenY=%d", bullet->GetY(), cameraY, bulletScreenY);
                shouldDelete = true;
            }
            // Xử lý xóa đạn
            if (shouldDelete) {
                delete bullet;
                it = bullets.erase(it);
            }
            else {
                ++it;
            }
        }
    }


    // Tạo thêm xe dân
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastCivilianSpawnTime > 1000) {
        int xPos = rand() % (800 - 50); // màn hình 800, xe 50
        cars.push_back(new CivilianCar(xPos, cameraY - 150)); // spawn cao hơn camera
        lastCivilianSpawnTime = currentTime;
    }

    background->Update();
}




void Game::Render() {
   
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    background->Render(renderer);
    for (auto car : cars) car->Render(renderer,cameraY);

    SDL_RenderPresent(renderer);
}

void Game::Clean() {
    for (auto car : cars) delete car;
    cars.clear();
    delete background;
    background = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	IMG_Quit();
    SDL_Quit();
    

}
