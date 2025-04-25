#pragma once
#include <SDL_mixer.h>
#include <string>

class SoundManager {
public:
    static SoundManager& GetInstance(); // Singleton pattern

    bool Init();
    void PlayBackgroundMusic();
    void StopBackgroundMusic();
    void PlayEngineRev();
    void PlayGunshot();
    void PlayExplosion();
    void Clean();

private:
    SoundManager(); // Constructor private để đảm bảo singleton
    ~SoundManager();

    // Các biến âm thanh
    Mix_Music* backgroundMusic;
    Mix_Chunk* engineRevSound;
    Mix_Chunk* gunshotSound;
    Mix_Chunk* explosionSound;

    // Không cho phép copy hoặc gán
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
};