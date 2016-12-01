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

void cpoles_to_spoles(struct cpoles *cpoles, struct spoles *spoles_out) {
    for (int i = 0; i < cpoles->num; i++) {
        struct cpole *cpole = cpoles->ob + i;
        struct spole *spole = spoles_out->ob + i;

        spole->p = (uint16_t) ((HALF_SCREEN_WIDTH * cpole->c / cpole->d) + HALF_SCREEN_WIDTH);
        spole->h = (uint16_t) (HALF_SCREEN_HEIGHT / cpole->d);
    }
    spoles_out->num = cpoles->num;

    /*
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
    spole->colour = COLOUR_END;

    */

}

void spoles_to_panels(struct spoles *spoles, struct panels *panels) {
    struct panel *panel = panels->ob;
    panels->num = 0;
    for (int i = 0; i < spoles->num; i++) {
        struct spole *left = spoles->ob + i;
        struct spole *right = spoles->ob + ((i + 1) % spoles->num);
        printf("%d %d", left->p, right->p);
        if (left->p < right->p) {
            panel->lp = left->p;
            panel->lh = left->h;
            panel->colour = left->colour;
            panel->rp = right->p;
            panel->rh = right->h;

            panel++;
            panels->num++;
            printf("\n");
        } else {
            printf(" X\n");
        }
    }
}

// bubble sort as we're only dealing with 5-10 items and do this only once per frame
void sort_panels_by_distance(struct panels *panels) {
    struct panel tmp;
    int8_t sorted = 0;
    while (!sorted) {
        sorted = 1; // hypothesise that the panels are sorted
        for (int i = 0; i < panels->num - 1; i++) {
            struct panel *a = panels->ob + i;
            struct panel *b = panels->ob + i + 1;
            if ((b->lh + b->rh) > (a->lh + a->rh)) {
                tmp = *a;
                *a = *b;
                *b = tmp;
                sorted = 0;
                printf("X");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

void panels_to_crit_points(uint16_t line, struct panels *panels, struct crit_points *crit_points_out) {
    struct crit_point *crit_point = crit_points_out->ob;
    for (int i = 0; i < panels->num; i++) {
        struct panel *panel = panels->ob + i;
        crit_point->p = panel->lp; // TODO: adjust either lp or rp dependent on line and height
        crit_point->panel_idx = i;
        crit_point->is_start = 1;
        crit_point++;
        crit_points_out->num++;

        crit_point->p = panel->rp; // TODO: adjust either lp or rp dependent on line and height
        crit_point->panel_idx = i;
        crit_point->is_start = 0;
        crit_point++;
        crit_points_out->num++;
    }
}

// TODO: faster algorithm needed - this is called on each scan line for 10-20 items
void sort_crit_points(struct crit_points *crit_points_io) {
    struct crit_point tmp;
    int8_t sorted = 0;
    while (!sorted) {
        sorted = 1; // hypothesise that the panels are sorted
        for (int i = 0; i < crit_points_io->num - 1; i++) {
            struct crit_point *a = crit_points_io->ob + i;
            struct crit_point *b = crit_points_io->ob + i + 1;
            if (b->p < a->p) {
                tmp = *a;
                *a = *b;
                *b = tmp;
                sorted = 0;
                printf("X");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

void crit_points_to_rles(struct crit_point *crit_points, struct rle *rles_out) {
}

int16_t sine(uint16_t ang) {
    // https://en.wikipedia.org/wiki/Bhaskara_I's_sine_approximation_formula
    return (int16_t) (4 * ang * (180 - ang) / (40500 - ang * (180 - ang)));
};
