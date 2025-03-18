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
const int SCREEN_WIDTH = 1140; 
const int SCREEN_HEIGHT = 640;
const int SCREEN_BPP = 32;
//color key
const int COLOR_KEY_R = 167;
const int COLOR_KEY_G = 175;
const int COLOR_KEY_B = 180;
const int RENDER_DRAW_COLOR = 0Xff;
#define TILE_SIZE 26
#define MAX_MAP_X 48
#define MAX_MAP_Y 24
#define MAX_TILES 32


typedef struct Input
{
	int left_;
	int right_;
	int up_;
	int down_;
	int jump_;
};





typedef struct MAP
{
    int start_x;
    int start_y;
    int max_x;
    int max_y;
    int tile[MAX_MAP_Y][MAX_MAP_X];
    string* file_name_;
};



#endif 