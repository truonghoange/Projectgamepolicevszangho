#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

class ExplosionEffect {
private:
  int x, y;
  int frameWidth, frameHeight;

protected: 
  int totalFrames;
  int currentFrame;

private:
  Uint32 lastFrameUpdate;
  SDL_Texture* texture;

public:
  ExplosionEffect(int x, int y, SDL_Renderer* renderer);
  ~ExplosionEffect();
  void Update();
  void Render(SDL_Renderer* renderer, int cameraY);
  bool IsFinished() const { return currentFrame >= totalFrames; }
  SDL_Rect GetRect() const { return { x, y, frameWidth, frameHeight }; }
  int GetX() const { return x; }
  int GetY() const { return y; }

  // Add these public getter methods to fix the errors
  int GetProtectedCurrentFrame() const { return currentFrame; }
  int GetProtectedTotalFrames() const { return totalFrames; }
};