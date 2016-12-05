/*
 * This is the code which is compiled for both amd64 and lpc810 targets.
 */

#include "dual.h"

int16_t mulsine(int16_t num, uint16_t ang) {
    ang = ang % 360; 
    // https://en.wikipedia.org/wiki/Bhaskara_I's_sine_approximation_formula
    int sign = 1;
    if (ang > 180) { 
	ang -= 180;
        sign = -1;
    }
    return (int16_t) (sign * ((int32_t)num * 4 * ang * (180 - ang) / (40500 - ang * (180 - ang))));
};

int16_t mulcos(int16_t num, uint16_t ang) {
    return mulsine(num, ang + 90);
}


void wpoles_to_cpoles(struct wpoles *wpoles, struct camera *camera, struct cpoles *cpoles_out) {
    for (int i = 0; i < wpoles->num; i++) {
        struct wpole *wpole = wpoles->ob + i;
        struct cpole *cpole = cpoles_out->ob + i;

        // TODO replace these assignments with a displacement and rotation by the camera position and facing
        //cpole->c = wpole->x - camera->x;
        //cpole->d = wpole->y - camera->y;
        int16_t x = wpole->x - camera->x;
        int16_t y = wpole->y - camera->y;
        cpole->c = mulcos(x, camera->facing) - mulsine(y, camera->facing);
        cpole->d = mulsine(x, camera->facing) + mulcos(y, camera->facing);
        cpole->colour = wpole->colour;
	printf(".\n");
    }
    cpoles_out->num = wpoles->num;
    printf("q\n");
}

void cpoles_to_cpanels(struct cpoles *cpoles, struct cpanels *cpanels_out) {
    struct cpanel *cpanel = cpanels_out->ob;
    cpanels_out->num = 0;
    for (int i = 0; i < cpoles->num; i++) {
        struct cpole *left = cpoles->ob + i;
        struct cpole *right = cpoles->ob + ((i + 1) % cpoles->num);
        printf("%d %d", left->c, right->d);

        // exclude cpanels where panel is behind viewer
        if (left->d < 0 && right->d < 0) {
            printf(" X\n");
            continue;
        }

        // TODO: modify endpoints to bring them within view

        cpanel->lc = left->c;
        cpanel->ld = left->d;
        cpanel->colour = left->colour;
        printf("colour == %d\n", cpanel->colour);
        cpanel->rc = right->c;
        cpanel->rd = right->d;

        cpanel++;
        cpanels_out->num++;
        printf("\n");
    }
}

void cpanels_to_panels(struct cpanels *cpanels, struct panels *panels_out) {
    panels_out->num = 0;
    struct panel *panel = panels_out->ob;
    for (int i = 0; i < cpanels->num; i++) {
	struct cpanel *cpanel = cpanels->ob + i;
        panel->lp = (uint16_t) ((HALF_SCREEN_WIDTH * cpanel->lc / cpanel->ld) + HALF_SCREEN_WIDTH);
        panel->lh = (uint16_t) (HALF_SCREEN_WIDTH * THOU / 2 / cpanel->ld);
        panel->rp = (uint16_t) ((HALF_SCREEN_WIDTH * cpanel->rc / cpanel->rd) + HALF_SCREEN_WIDTH);
        panel->rh = (uint16_t) (HALF_SCREEN_WIDTH * THOU / 2 / cpanel->rd);
	panel->colour = cpanel->colour;
	printf("%d %d %d %d %d\n", panel->lp, panel->lh, panel->rp, panel->rh, panel->colour);

        // exclude back faces
        if (panel->lp >= panel->rp) {
            printf(" X\n");
            continue;
        }

	
	panel++;
    }
    panels_out->num = panel - panels_out->ob;
}

void cpoles_to_spoles(struct cpoles *cpoles, struct spoles *spoles_out) {
    for (int i = 0; i < cpoles->num; i++) {
        struct cpole *cpole = cpoles->ob + i;
        struct spole *spole = spoles_out->ob + i;

	printf("%d %d\n", cpole->c, cpole->d);
        spole->p = (uint16_t) ((HALF_SCREEN_WIDTH * cpole->c / cpole->d) + HALF_SCREEN_WIDTH);
	printf("w\n");
        spole->h = (uint16_t) (HALF_SCREEN_WIDTH * THOU / 2 / cpole->d);
	printf("y\n");
        spole->colour = cpole->colour;
    }
    spoles_out->num = cpoles->num;
}

// TODO: this needs to discard or modify panels which would wholly or partially be outside of the viewing area
void spoles_to_panels(struct spoles *spoles, struct panels *panels) {
    struct panel *panel = panels->ob;
    panels->num = 0;
    for (int i = 0; i < spoles->num; i++) {
        struct spole *left = spoles->ob + i;
        struct spole *right = spoles->ob + ((i + 1) % spoles->num);
        printf("%d %d", left->p, right->p);

        // exclude back faces
        if (left->p >= right->p) {
            printf(" X\n");
            continue;
        }

        // TODO: exclude

        panel->lp = left->p;
        panel->lh = left->h;
        panel->colour = left->colour;
        printf("colour == %d\n", panel->colour);
        panel->rp = right->p;
        panel->rh = right->h;

        panel++;
        panels->num++;
        printf("\n");
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
    uint16_t offset = abs(line - SCREEN_HEIGHT / 2); // offset from centre line
    struct crit_point *crit_point = crit_points_out->ob;
    crit_points_out->num = 0;
    for (int i = 0; i < panels->num; i++) {
        struct panel *panel = panels->ob + i;
        if (panel->lh < offset && panel->rh < offset) continue;  // this panel is too low to affect this scanline

        int16_t w = panel->rp - panel->lp;
        int16_t h = panel->lh - panel->rh;
        int16_t ladj = 0;
        int16_t radj = 0;

        if (offset != 0 && panel->lh >= offset && panel->rh < offset) {
            ladj = (offset - panel->rh) * w / h;
            //printf("ladj == %d\n", ladj);
        }
        if (offset != 0 && panel->lh < offset && panel->rh >= offset) {
            radj = (offset - panel->lh) * w / h;
            //printf("radj == %d\n", radj);
        }

        crit_point->p = panel->lp - radj;
        crit_point->panel_idx = i;
        //crit_point->is_start = 1; // each panel will start and stop exactly once, and the start will not be right of the stop
        crit_point++;
        crit_points_out->num++;

        crit_point->p = panel->rp - ladj;
        crit_point->panel_idx = i;
        //crit_point->is_start = 0; // each panel will start and stop exactly once, and the start will not be right of the stop
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
                //printf("X");
            } else {
                //printf(".");
            }
        }
        //printf("\n");
    }
}

void crit_points_to_changes(struct crit_points *crit_points, struct panels *sorted_panels, struct changes *changes_out) {
    uint32_t tracker = 0x00000000;
    struct change *change = changes_out->ob;
    changes_out->num = 0;
    uint8_t last_colour = 0;

    change->start = 0;
    change->colour = 0; // background
    change++;
    changes_out->num++;

    for (int i = 0; i < crit_points->num; i++) {
        //printf("i == %d\n", i);
        // loop through all crit_points at p
        struct crit_point *crit_point = crit_points->ob + i;
        int16_t p = crit_point->p;
        for (; crit_point->p == p && i < crit_points->num; crit_point++, i++) {
            tracker ^= 1 << crit_point->panel_idx; // each panel will start and stop exactly once, and the start will not be right of the stop
            //printf("  i == %d, tracker == %8.8x\n", i, tracker);
        }
        i--; // reverse overshoot
        uint8_t colour = 0;
        for (int b = 0; b < 32; b++) {
            //printf("  b == %d\n", b);
            if (tracker & (1 << b)) {
                colour = (sorted_panels->ob + b)->colour;
                //printf("  colour == %d\n", colour);
                break;
            }
            if (b == 31) {
                colour = 0; // background
                //printf("  colour == %d (background)\n", colour);
            }
        }
        if (colour != last_colour) {
            change->start = p;
            change->colour = colour;
            change++;
            changes_out->num++;
            last_colour = colour;
        }
    }
}

