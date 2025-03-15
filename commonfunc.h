#ifndef COMMON_FUNCTION_H_
#define COMMON_FUNCTION_H_

#include<Windows.h>
#include<string>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
using namespace std;

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_screen = NULL;
static SDL_Event g_event;
static SDL_Surface* g_screen_surface = NULL;
//screen
const int SCREEN_WIDTH = 1248;
const int SCREEN_HEIGHT = 624;
const int SCREEN_BPP = 32;
//color key
const int COLOR_KEY_R = 167;
const int COLOR_KEY_G = 175;
const int COLOR_KEY_B = 180;
const int RENDER_DRAW_COLOR = 155;
#define TILE_SIZE 26
#define MAX_MAP_X 48
#define MAX_MAP_Y 24

enum Direction
{
    UP , DOWN, LEFT, RIGHT, NONE
};
struct Position
{
    int x;
    int y;

    Position(int x_, int y_) : x(x_), y(y_) {}

    Position move(Direction d)
    {
        const int dx[] = { 0,0,-1,1 };
        const int dy[] = { -1,1,0,0 };
        return Position(x + dx[d], y + dy[d]);
    }


    bool operator==(Position p) {
        return x == p.x && y == p.y;
    }
};



struct Map
{
    int start_x;
    int start_y;
    int max_x;
    int max_y;
    int tile[MAX_MAP_Y][MAX_MAP_X];
    string file_name_;
};



#endif 