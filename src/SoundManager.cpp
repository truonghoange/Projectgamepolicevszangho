#include "SoundManager.h"
#include <SDL.h>

SoundManager::SoundManager() : backgroundMusic(nullptr), engineRevSound(nullptr),
gunshotSound(nullptr), explosionSound(nullptr), engineRevChannel(-1) {
}

SoundManager::~SoundManager() {
    Clean();
}

SoundManager& SoundManager::GetInstance() {
    static SoundManager instance;
    return instance;
}

bool SoundManager::Init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Không thể khởi tạo SDL_mixer: %s", Mix_GetError());
        return false;
    }

    backgroundMusic = Mix_LoadMUS("music/soundback.wav");
    if (!backgroundMusic) {
        SDL_Log("Không thể tải background music: %s", Mix_GetError());
        return false;
    }
    engineRevSound = Mix_LoadWAV("music/carsound.wav");
    if (!engineRevSound) {
        SDL_Log("Không thể tải engine rev sound: %s", Mix_GetError());
        return false;
    }
    gunshotSound = Mix_LoadWAV("music/shot.wav");
    if (!gunshotSound) {
        SDL_Log("Không thể tải gunshot sound: %s", Mix_GetError());
        return false;
    }
    explosionSound = Mix_LoadWAV("music/ChunkyExplosion.wav");
    if (!explosionSound) {
        SDL_Log("Không thể tải explosion sound: %s", Mix_GetError());
        return false;
    }

    return true;
}

void SoundManager::PlayBackgroundMusic() {
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

void SoundManager::StopBackgroundMusic() {
    Mix_HaltMusic();
}

void SoundManager::PlayEngineRev() {
    if (engineRevChannel == -1 || !Mix_Playing(engineRevChannel)) {
        engineRevChannel = Mix_PlayChannel(-1, engineRevSound, -1); // Lặp vô hạn
        if (engineRevChannel == -1) {
            SDL_Log("Không thể phát engine rev sound: %s", Mix_GetError());
        }
    }
}

void SoundManager::StopEngineRev() {
    if (engineRevChannel != -1 && Mix_Playing(engineRevChannel)) {
        Mix_HaltChannel(engineRevChannel);
        engineRevChannel = -1;
    }
}

void SoundManager::PlayGunshot() {
    Mix_PlayChannel(-1, gunshotSound, 0);
}

void SoundManager::PlayExplosion() {
    Mix_PlayChannel(-1, explosionSound, 0);
}

void SoundManager::Clean() {
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
    if (engineRevSound) {
        Mix_FreeChunk(engineRevSound);
        engineRevSound = nullptr;
    }
    if (gunshotSound) {
        Mix_FreeChunk(gunshotSound);
        gunshotSound = nullptr;
    }
    if (explosionSound) {
        Mix_FreeChunk(explosionSound);
        explosionSound = nullptr;
    }
    Mix_CloseAudio();
}