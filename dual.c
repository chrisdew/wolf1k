/*
 * This is the code which is compiled for both amd64 and lpc810 targets.
 */

#include "dual.h"

int16_t mulsine(int16_t num, uint8_t ang) {
    // https://en.wikipedia.org/wiki/Bhaskara_I's_sine_approximation_formula
    int sign = 1;
    if (ang > 128) { 
	ang -= 128;
        sign = -1;
    }
    return (int16_t) (sign * ((int32_t)num * 4 * ang * (128 - ang) / (20480 - ang * (128 - ang))));
};

int16_t mulcos(int16_t num, uint8_t ang) {
    return mulsine(num, ang + 64);
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
        cpole->c = mulcos(x, -camera->facing) - mulsine(y, -camera->facing);
        cpole->d = mulsine(x, -camera->facing) + mulcos(y, -camera->facing);
        cpole->colour = wpole->colour;
    }
    cpoles_out->num = wpoles->num;
}

void cpoles_to_cpanels(struct cpoles *cpoles, struct cpanels *cpanels_out) {
    struct cpanel *cpanel = cpanels_out->ob;
    cpanels_out->num = 0;
    for (int i = 0; i < cpoles->num; i++) {
        struct cpole *left = cpoles->ob + i;
        struct cpole *right = cpoles->ob + ((i + 1) % cpoles->num);
        //printf("%d %d", left->c, right->d);

        // exclude cpanels where panel is behind viewer
        if (left->d < 0 && right->d < 0) {
            //printf(" X\n");
            continue;
        }

        // TODO: modify endpoints to bring them within view

        cpanel->lc = left->c;
        cpanel->ld = left->d;
        cpanel->colour = left->colour;
        //printf("colour == %d\n", cpanel->colour);
        cpanel->rc = right->c;
        cpanel->rd = right->d;

        cpanel++;
        cpanels_out->num++;
        //printf("\n");
    }
}

struct cpanel cpanel_clip(struct cpanel cpanel) {
    struct cpanel ret = {};

    int16_t lc = ret.lc = cpanel.lc;
    int16_t ld = ret.ld = cpanel.ld;
    int16_t rc = ret.rc = cpanel.rc;
    int16_t rd = ret.rd = cpanel.rd;
    ret.colour = cpanel.colour;

    int8_t left_in_view = ld > ABS(lc);
    int8_t right_in_view = rd > ABS(rc);

    if (cpanel.ld < 1 && cpanel.rd < 1) {
        //printf("behind you\n");
        return (struct cpanel) {};
    }

    if (!left_in_view && !right_in_view) {
        if (!(ld > 0 && rd > 0 && lc < 0 && rc > 0)) {
            //printf("out of sight\n");
            return (struct cpanel) {};
        }
    }

    if (!right_in_view && rd < 1) {
        int nom = (rd - ld);
        int denom = (rc - lc);
            if (denom != 0 && (nom / denom) != 1) {
                int m = THOU * nom / denom;
                //printf("rr normal condition %d/%d = %d ", nom, denom, m);
                ret.rc = (THOU * ld - m * lc) / (THOU - m);
                ret.rd = ret.rc;
            } else if (nom != 0 && (denom / nom) != 1) {
                int m = THOU * denom / nom;
                //printf("rr reversed condition %d ", m);
                ret.rc = (THOU * lc - m * ld) / (THOU - m);
                ret.rd = ret.rc;
            } else {
                //printf("rr odd condition ");
            }
        /*
        } else {
            if (denom != 0 && (nom / denom) != 1) {
                int m = THOU * nom / denom;
                //printf("rl normal condition %d/%d = %d ", nom, denom, m);
                ret.rc = (THOU * ld - m * lc) / (THOU - m);
                ret.rd = ret.rc;
            } else if (nom != 0 && (denom / nom) != 1) {
                int m = THOU * denom / nom;
                //printf("rl reversed condition %d ", m);
                ret.rc = (THOU * lc - m * ld) / (THOU - m);
                ret.rd = ret.rc;
            } else {
                //printf("rl odd condition ");
            }
        }
         */
        //printf("%d %d\n", ret.rc, ret.rd);
    }

    if (!left_in_view && ld < 1) {
        int nom = (rd - ld);
        int denom = (rc - lc);
        if (denom != 0 && (nom / denom) != -1) {
            int m = THOU * nom / denom;
            //printf("l normal condition %d/%d = %d ", nom, denom, m);
            ret.lc = -(THOU * ld - m * lc) / (THOU + m);
            ret.ld = -ret.lc;
        } else if (nom != 0 && (denom / nom) != -1) {
            int m = THOU * denom / nom;
            //printf("l reversed condition %d ", m);
            ret.lc = (THOU * lc - m * ld) / (THOU + m);
            ret.ld = -ret.lc;
        } else {
            //printf("l odd condition ");
        }
        //printf("%d %d\n", ret.lc, ret.ld);
    }
    return ret;
}

void cpanel_print(char *prefix, struct cpanel cpanel) {
    //printf("%s: panel(lc:%d,ld:%d,rc:%d,rd:%d,colour:%d)\n", prefix, cpanel.lc, cpanel.ld, cpanel.rc, cpanel.rd, cpanel.colour);

}

void cpanels_to_panels(struct cpanels *cpanels, struct panels *panels_out) {
    panels_out->num = 0;
    struct panel *panel = panels_out->ob;
    for (int i = 0; i < cpanels->num; i++) {
	    struct cpanel *cpanel = cpanels->ob + i;

        cpanel_print("before", *cpanel);
        int cross = cpanel->lc * cpanel->rd - cpanel->ld * cpanel->rc;
        //printf(" X %d\n", cross);
        if (cross >= 0) {
            continue;
        }

        struct cpanel clipped = cpanel_clip(*cpanel);
        cpanel_print(" after", clipped);

        if (clipped.colour == 0) { // mark for cpanels to discard
            continue;
        }

        panel->lp = (uint16_t) ((HALF_SCREEN_WIDTH * clipped.lc / clipped.ld) + HALF_SCREEN_WIDTH);
        panel->lh = (uint16_t) (HALF_SCREEN_WIDTH * THOU / 2 / clipped.ld);
        panel->rp = (uint16_t) ((HALF_SCREEN_WIDTH * clipped.rc / clipped.rd) + HALF_SCREEN_WIDTH);
        panel->rh = (uint16_t) (HALF_SCREEN_WIDTH * THOU / 2 / clipped.rd);
	    panel->colour = cpanel->colour;
	    //printf("%d %d %d %d %d\n", panel->lp, panel->lh, panel->rp, panel->rh, panel->colour);

        if (panel->lh < 0 || panel->rh < 0) {
            exit(-1);
        }
        panel++;
    }
    panels_out->num = panel - panels_out->ob;
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
            if (MIN(b->lh, b->rh) > MIN(a->lh, a->rh)) {
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

void panels_to_crit_points(uint16_t line, struct panels *panels, struct crit_points *crit_points_out) {
    uint16_t offset = ABS(line - SCREEN_HEIGHT / 2); // offset from centre line
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

        crit_point->p = (panel->lp > radj) ? panel->lp - radj : 0;
        crit_point->panel_idx = i;
        //crit_point->is_start = 1; // each panel will start and stop exactly once, and the start will not be right of the stop
        crit_point++;
        crit_points_out->num++;

        crit_point->p = (panel->rp > ladj) ? panel->rp - ladj : 0;
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

