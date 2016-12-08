/*
 * This is the code which is compiled to run on a desktop machine.
 */

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "amd64.h"

static struct wpoles wpoles = {
        MAX_WALLS,
        {
                {0 * THOU, 3 * THOU, CYAN},
                {0 * THOU, 2 * THOU, YELLOW},
                {1 * THOU, 2 * THOU, GREEN},
                {1 * THOU, 3 * THOU, RED},
                {2 * THOU, 3 * THOU, CYAN},
                {2 * THOU, 1 * THOU, YELLOW},
                {-1 * THOU, 1 * THOU, GREEN},
                {-1 * THOU, 3 * THOU, RED},
        }
};
struct camera camera = {.x = 0, .y = 0, .facing = 0};
static struct cpoles cpoles;
static struct spoles spoles;
static struct cpanels cpanels;
static struct panels panels;
static struct crit_points crit_points;
static struct changes changes;

void draw_scanline(SDL_Renderer *renderer, uint16_t line, struct changes *changes) {
    uint16_t p = 0;
    for (int i = 0; i < changes->num; i++) {
        struct change *change = changes->ob + i;
        SDL_SetRenderDrawColor(renderer, 255 * !!(change->colour & 0b100), 255 * !!(change->colour & 0b010), 255 * !!(change->colour & 0b001), 255);
        SDL_RenderDrawLine(renderer, change->start, line, SCREEN_WIDTH - 1, line);
    }
}

void draw_frame(SDL_Renderer *renderer, struct panels *panels, struct crit_points* crit_points, struct changes *changes) {
    //SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    //SDL_RenderClear(renderer);

    for (uint16_t line = 0; line < SCREEN_HEIGHT; line++) {
        panels_to_crit_points(line, panels, crit_points);
        sort_crit_points(crit_points);
        //printf("crit_points->num == %d\n", crit_points->num);
        crit_points_to_changes(crit_points, panels, changes);
        //printf("changes->num == %d\n", changes->num);
        draw_scanline(renderer, line, changes);
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

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return -1;
    window = SDL_CreateWindow( "Server", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    if (window == NULL) return -2;
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == NULL) return -3;

    wpoles_to_cpoles(&wpoles, &camera, &cpoles);
    cpoles_to_spoles(&cpoles, &spoles);
    draw(renderer, &spoles);
    SDL_Delay(1000);

/*
    spoles_to_panels(&spoles, &panels);
    sort_panels_by_distance(&panels);
    printf("panels.num == %d\n", panels.num);
*/

    for (int16_t ang = 0; ang < 360; ang += 45) {
        printf("ang: %d, sine: %d, cos: %d\n", ang, mulsine(1000, ang), mulcos(1000, ang));
    }

    int loop = 1;
    int16_t dx, dy, mc, ms;
    while(loop) {
        printf("loop\n");
        wpoles_to_cpoles(&wpoles, &camera, &cpoles);
        cpoles_to_cpanels(&cpoles, &cpanels);
        cpanels_to_panels(&cpanels, &panels);
        sort_panels_by_distance(&panels);
        printf("panels.num == %d\n", panels.num);
        draw_frame(renderer, &panels, &crit_points, &changes);
        SDL_Delay(1600);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        printf("right\n");
                        camera.facing = (camera.facing - 30 + 360) % 360;
                        printf("x: %d, y: %d, facing: %d\n", camera.x, camera.y, camera.facing);
                        break;
                    case SDLK_LEFT:
                        printf("left\n");
                        camera.facing = (camera.facing + 30 + 360) % 360;
                        printf("x: %d, y: %d, facing: %d\n", camera.x, camera.y, camera.facing);
                        break;
                    case SDLK_DOWN:
                        printf("down\n");
                        mc = mulcos(100, camera.facing);
                        ms = mulsine(100, camera.facing);
                        dx = -ms;
                        dy = mc;
                        camera.x -= dx;
                        camera.y -= dy;
                        printf("mc: %d, ms: %d, dx: %d, dy: %d, x: %d, y: %d, facing: %d\n", mc, ms, dx, dy, camera.x, camera.y, camera.facing);
                        break;
                    case SDLK_UP:
                        printf("up\n");
                        mc = mulcos(100, camera.facing);
                        ms = mulsine(100, camera.facing);
                        dx = -ms;
                        dy = mc;
                        camera.x += dx;
                        camera.y += dy;
                        printf("mc: %d, ms: %d, dx: %d, dy: %d, x: %d, y: %d, facing: %d\n", mc, ms, dx, dy, camera.x, camera.y, camera.facing);
                        break;
                    default :
                        break;
                }
            }
        }
    }

    return 0;
}
