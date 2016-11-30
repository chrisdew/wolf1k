/*
 * This is the code which is compiled for both amd64 and lpc810 targets.
 */

#include "dual.h"

struct wpole wpoles[MAX_WALLS_PLUS_ONE] = {
    {-1,3,1},
    {-1,1,2},
    { 2,1,3},
    { 2,3,4},
    { 1,3,5},
    { 1,2,6},
    { 0,2,7},
    { 0,3,8},
    {-1,3,1} // must be same as first (a loop of wall poles)
};
struct camera camera = {.x = 0, .y = 0, .facing = NORTH};

void cpoles_to_spoles(struct cpole *cpoles, struct spole *spoles_out) {
    struct cpole *cpole = cpoles; // only one declarations of one type may be made in a for loop
    struct spole *spole = spoles_out; // so we make both here

    // transform to screen coordinates
    do {
        spole->p = (uint16_t) ((HALF_SCREEN_WIDTH * cpole->c / cpole->d) + HALF_SCREEN_WIDTH);
        spole->h = (uint16_t) (HALF_SCREEN_HEIGHT / cpole->d);

        cpole++;
        spole++;
    } while (cpole->c != cpoles->c || cpole->d != cpoles->d);
    *spole = *spoles_out; // tie the knot

    /*
    // set up prev pointers
    spoles_out[0].prev = &spoles_out[MAX_WALLS - 1];
    for (int i = 1; i < MAX_WALLS; i++) {
        spoles_out[i].prev = &spoles_out[i - 1];
    }

    // set up next pointers
    for (int i = 0; i < MAX_WALLS - 1; i++) {
        spoles_out[i].prev = &spoles_out[i - 1];
    }
    spoles_out[MAX_WALLS - 1].next = &spoles_out[0];

    // sort by screen position
    int sorted = 0;
    spole
    while (!sorted) {
        sorted = 1; // theorise that the array is sorted
        for (int i = 1; i < MAX_WALLS; i++) {
            if (spoles_out[i-1].p > spoles_out[i].p) {

            }
        }
    }
    */

}

void spoles_to_panels(struct spole *spoles, struct panel *panels) {
    struct panel *panel = panels;
    struct spole *left = spoles;
    struct spole *right = spoles + 1;
    do {
        if (left->p < right->p) {
            panel->lp = left->p;
            panel->lh = left->h;
            panel->colour = left->colour;
            panel->rp = right->p;
            panel->rh = right->h;
        }
    } while (left->h != spoles->h || left->p != spoles->p);
    panel->colour = COLOUR_END; // terminate the list
}

void panels_to_lpoles(uint16_t line, struct panel *panels, struct lpole *lpoles_out) {
}

void sort_lpoles(struct lpole *lpoles_io) {
}

void lpoles_to_rles(struct lpole *lpoles, struct rle *rles_out) {
}

int16_t sine(uint16_t ang) {
    // https://en.wikipedia.org/wiki/Bhaskara_I's_sine_approximation_formula
    return (int16_t) (4 * ang * (180 - ang) / (40500 - ang * (180 - ang)));
};
