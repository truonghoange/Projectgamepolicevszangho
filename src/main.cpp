﻿#include "commonfunc.h"
#include "BaseObject.h"
#include "charobject.h"
#include "MapofGame.h"  

BaseObject g_background;

bool InitData()
{
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    g_window = SDL_CreateWindow("Game Cpp",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (g_window == NULL) {
        success = false;
    }
    else {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL) success = false;
        else {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) success = false;
        }
    }
    return success;
}

bool LoadBackground() {
    bool ret = g_background.Load_Img("image//nen.png", g_screen);
    if (ret == false) return false;
    return true;
}

void close() {
    g_background.Free();
    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;
    SDL_DestroyWindow(g_window);
    g_window = NULL;

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (InitData() == false) return -1;
    if (LoadBackground() == false) return -1;

    // Load map
    TileMap game_map(g_screen);
    if (!game_map.loadMap("F:/Vs2022/Projectgamepolicevszangho/Mapgame/map.tmx",
        "F:/Vs2022/Projectgamepolicevszangho/Mapgame/forest_tileset_night.tsx")) {
        close();
        return -1;
    }

    CharObject g_char;
    if (!g_char.Load_Img("image//walk_right.png", g_screen)) {
        close();
        return -1;
    }
    g_char.set_clips();

    bool is_quit = false;
    while (!is_quit) {
        while (SDL_PollEvent(&g_event) != 0) {
            if (g_event.type == SDL_QUIT) {
                is_quit = true;
            }
            g_char.HandleInputAction(g_event, g_screen);
        }

        SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
        SDL_RenderClear(g_screen);

        game_map.render();
        g_char.Show(g_screen);

        SDL_RenderPresent(g_screen);
    }

    close();
    return 0;
}