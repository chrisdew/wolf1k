/*
 * This is the code which is compiled to run on a desktop machine.
 */

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "amd64.h"

static struct cpoles cpoles = { // TODO: this should be calculated from "wpoles" and "camera"
        MAX_WALLS,
        {
                {0, 3, CYAN},
                {0, 2, YELLOW},
                {1, 2, CYAN},
                {1, 3, YELLOW},
                {2, 3, CYAN},
                {2, 1, YELLOW},
                {-1, 1, CYAN},
                {-1, 3, YELLOW},
        }
};
static struct spoles spoles; // sorted by screen position left to right
static struct panels panels;
static struct crit_points crit_points;
static struct rle rles[MAX_WALLS_PLUS_ONE];

void draw_scanline(SDL_Renderer *renderer, uint16_t line, struct rle *rles) {
    uint16_t p = 0;
    struct rle *rle = rles;
    while (rle->colour != COLOUR_END) {
        SDL_SetRenderDrawColor(renderer, 255 * !!(rle->colour & 0b100), 255 * !!(rle->colour & 0b010), 255 * !!(rle->colour & 0b001), 255);
        SDL_RenderDrawLine(renderer, rle->start, line, SCREEN_WIDTH - 1, line);
    }
}

void draw_frame(SDL_Renderer *renderer, struct panels *panels, struct crit_points* crit_points) {
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderClear(renderer);

    for (uint16_t line = 319; line < 320; line++) {
        panels_to_crit_points(line, panels, crit_points);
        sort_crit_points(crit_points);
        printf("crit_points->num == %d\n", crit_points->num);
	    //sort_crit_points(crit_points);
        //crit_points_to_rles(crit_points, rles);
        //draw_scanline(renderer, line, rles);
    }

    SDL_RenderPresent(renderer);
}

void draw(SDL_Renderer *renderer, struct spoles *spoles) {
    // this will become void draw_frame(SDL_Renderer *renderer, struct panel *panels)
    // which will call void draw_scanline(SDL_Renderer *renderer, uint16_t line, struct rle *rles) once per scanline
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );

    // TODO: transform these wall poles into an rle encoded scanline for the horizontal centre line of the screen

    for (int i = 0; i < spoles->num; i++) {
        struct spole *spole = spoles->ob + i;
        printf("%d %d\n", spole->p, spole->h);
        SDL_RenderDrawLine(renderer, spole->p, HALF_SCREEN_HEIGHT - spole->h, spole->p, HALF_SCREEN_HEIGHT + spole->h);
    }

    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );
    //SDL_RenderDrawLine(renderer, 10, 10, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10);
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    printf("%lu\n", sizeof(struct rle));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return -1;
    window = SDL_CreateWindow( "Server", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    if (window == NULL) return -2;
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == NULL) return -3;

    cpoles_to_spoles(&cpoles, &spoles);
    draw(renderer, &spoles);
    SDL_Delay(1000);

    spoles_to_panels(&spoles, &panels);
    sort_panels_by_distance(&panels);
    printf("panels.num == %d\n", panels.num);
    draw_frame(renderer, &panels, &crit_points);
    SDL_Delay(1000);


    return 0;
}
