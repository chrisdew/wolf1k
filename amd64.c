/*
 * This is the code which is compiled to run on a desktop machine.
 */

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "amd64.h"

static struct cpole cpoles[MAX_WALLS_PLUS_ONE] = { // TODO: this should be calculated from "wpoles" and "camera"
        {-1,3,YELLOW},
        {-1,1,CYAN},
        { 2,1,YELLOW},
        { 2,3,CYAN},
        { 1,3,YELLOW},
        { 1,2,CYAN},
        { 0,2,YELLOW},
        { 0,3,CYAN},
        {-1,3,YELLOW} // must be same as first (a loop of wall poles)
};
static struct spole spoles[MAX_WALLS_PLUS_ONE]; // sorted by screen position left to right
static struct panel panels[MAX_WALLS_PLUS_ONE];
static struct lpanel lpanels[MAX_WALLS_PLUS_ONE];
static struct lpole lpoles[MAX_WALLS_PLUS_ONE];
static struct rle rles[MAX_WALLS_PLUS_ONE];

void draw_scanline(SDL_Renderer *renderer, uint16_t line, struct rle *rles) {

}

void draw_frame(SDL_Renderer *renderer, struct panel *panels) {
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderClear(renderer);

    for (uint16_t line = 0; line < SCREEN_HEIGHT; line++) {
        panels_to_lpoles(line, panels, lpoles);
	sort_lpoles(lpoles);
        lpoles_to_rles(lpoles, rles);
        draw_scanline(renderer, line, rles);
    }
}

/*
void draw(SDL_Renderer *renderer, struct spole *spoles) {
    // this will become void draw_frame(SDL_Renderer *renderer, struct panel *panels)
    // which will call void draw_scanline(SDL_Renderer *renderer, uint16_t line, struct rle *rles) once per scanline
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );

    // TODO: transform these wall poles into an rle encoded scanline for the horizontal centre line of the screen

    for (struct spole *spole = spoles; spole->p != -0x8000 || spole->h != -0x8000; spole++) {
        printf("%d %d\n", spole->p, spole->h);
        SDL_RenderDrawLine(renderer, spole->p, HALF_SCREEN_HEIGHT - spole->h, spole->p, HALF_SCREEN_HEIGHT + spole->h);
    }

    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );
    //SDL_RenderDrawLine(renderer, 10, 10, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10);
    SDL_RenderPresent(renderer);
}
*/

int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;

    printf("%lu\n", sizeof(struct rle));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return -1;
    window = SDL_CreateWindow( "Server", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    if (window == NULL) return -2;
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == NULL) return -3;

    cpoles_to_spoles(cpoles, spoles);
    spoles_to_panels(spoles, panels);
    draw_frame(renderer, panels);

    SDL_Delay(4000);

    return 0;
}
