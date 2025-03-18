//load map from file xml
#pragma once

#include"commonfunc.h"
#include "tinyxml2.h"



class TileMap {
private:
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;
    int tileWidth;
    int tileHeight;
    int mapWidth;
    int mapHeight;
    std::vector<TileLayer> layers;

public:
    TileMap(SDL_Renderer* rend);
    ~TileMap();

    bool loadMap(const std::string& filename, const std::string& tilesetPath);
    void render();
};