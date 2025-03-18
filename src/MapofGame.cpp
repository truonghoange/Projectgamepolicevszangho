#include "MapofGame.h"
#include <iostream>

TileMap::TileMap(SDL_Renderer* rend) 
    : renderer(rend), tilesetTexture(nullptr), tileWidth(0), tileHeight(0), mapWidth(0), mapHeight(0) {}
TileMap::~TileMap() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
    }
}

bool TileMap::loadMap(const std::string& filename, const std::string& tilesetPath) {
    // Load TMX file
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cout << "Failed to load map file: " << filename << std::endl;
        return false;
    }

    // Get map properties
    tinyxml2::XMLElement* mapElement = doc.FirstChildElement("map");
    mapWidth = mapElement->IntAttribute("width");
    mapHeight = mapElement->IntAttribute("height");
    tileWidth = mapElement->IntAttribute("tilewidth");
    tileHeight = mapElement->IntAttribute("tileheight");

    // Load tileset (.tsx file)
    tinyxml2::XMLDocument tilesetDoc;
    if (tilesetDoc.LoadFile(tilesetPath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cout << "Failed to load tileset file: " << tilesetPath << std::endl;
        return false;
    }

    // Get the image path from the tileset
    tinyxml2::XMLElement* tilesetElement = tilesetDoc.FirstChildElement("tileset");
    tinyxml2::XMLElement* imageElement = tilesetElement->FirstChildElement("image");
    const char* imagePath = imageElement->Attribute("source");

    // Adjust the image path (since .tsx file might have a relative path)
    std::string tilesetDir = tilesetPath.substr(0, tilesetPath.find_last_of("/\\"));
    std::string fullImagePath = tilesetDir + "/" + imagePath;

    // Log the full image path for debugging
    std::cout << "Attempting to load tileset image: " << fullImagePath << std::endl;

    // Load tileset image
    SDL_Surface* tilesetSurface = IMG_Load(fullImagePath.c_str());
    if (!tilesetSurface) {
        std::cout << "Failed to load tileset image: " << fullImagePath << " - " << IMG_GetError() << std::endl;
        return false;
    }
    tilesetTexture = SDL_CreateTextureFromSurface(renderer, tilesetSurface);
    SDL_FreeSurface(tilesetSurface);
    if (!tilesetTexture) {
        std::cout << "Failed to create texture from tileset: " << SDL_GetError() << std::endl;
        return false;
    }

    // Load layers
    tinyxml2::XMLElement* layerElement = mapElement->FirstChildElement("layer");
    while (layerElement) {
        TileLayer layer;
        layer.name = layerElement->Attribute("name");
        layer.width = layerElement->IntAttribute("width");
        layer.height = layerElement->IntAttribute("height");

        // Get tile data
        tinyxml2::XMLElement* dataElement = layerElement->FirstChildElement("data");
        std::string dataText = dataElement->GetText();
        std::vector<int>& tiles = layer.data;

        // Parse CSV data
        size_t pos = 0;
        while ((pos = dataText.find(",")) != std::string::npos) {
            std::string token = dataText.substr(0, pos);
            tiles.push_back(std::stoi(token));
            dataText.erase(0, pos + 1);
        }
        tiles.push_back(std::stoi(dataText)); // Last tile

        layers.push_back(layer);
        layerElement = layerElement->NextSiblingElement("layer");
    }

    return true;
}

void TileMap::render() {
    for (const auto& layer : layers) {
        for (int y = 0; y < layer.height; y++) {
            for (int x = 0; x < layer.width; x++) {
                int tileId = layer.data[y * layer.width + x];
                if (tileId > 0) {
                    int tilesetCols = 8; // Adjust this based on your tileset
                    int srcX = ((tileId - 1) % tilesetCols) * tileWidth;
                    int srcY = ((tileId - 1) / tilesetCols) * tileHeight;

                    SDL_Rect srcRect = { srcX, srcY, tileWidth, tileHeight };
                    SDL_Rect dstRect = { x * tileWidth, y * tileHeight, tileWidth, tileHeight };
                    SDL_RenderCopy(renderer, tilesetTexture, &srcRect, &dstRect);
                }
            }
        }
    }
}