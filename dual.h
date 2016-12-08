#ifndef __DUAL_H
#define __DUAL_H

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define THOU 1000

#define NORTH 0
#define EAST  90
#define SOUTH 180
#define WEST  270

#define SCREEN_WIDTH       640
#define HALF_SCREEN_WIDTH  (SCREEN_WIDTH / 2)
#define SCREEN_HEIGHT      480
#define HALF_SCREEN_HEIGHT (SCREEN_HEIGHT / 2)

#define MAX_WALLS          8
#define MAX_WALLS_PLUS_ONE (MAX_WALLS + 1)
#define MAX_COLOUR_CHANGES 10

#define BLACK   0b000
#define BLUE    0b001
#define GREEN   0b010
#define CYAN    0b011
#define RED     0b100
#define MAGENTA 0b101
#define YELLOW  0b110
#define WHITE   0b111

#define COLOUR_END 0xFF

// w - x and y are world coordinates
// c - c and d are camera coordinates
// s - p and q are screen coordinates

struct wpole {
    int16_t x;
    int16_t y;
    int8_t colour;
};

struct wpoles {
    int8_t num;
    struct wpole ob[MAX_WALLS];
};

struct camera {
    int16_t x;
    int16_t y;
    int16_t facing;
};

struct cpole {
    int16_t c;
    int16_t d;
    int8_t colour;
};

struct cpoles {
    int8_t num;
    struct cpole ob[MAX_WALLS];
};


struct spole { // screen pole
    int16_t p; // horizontal position
    int16_t h; // vertical height
    int8_t colour;
    int8_t offscreen;
    //struct spole *next;
    //struct spole *prev;
};

struct spoles {
    int8_t num;
    struct spole ob[MAX_WALLS];
};


/*
struct ppole { // (screen) panel poles
    int16_t p; // horizontal position
    int16_t h; // vertical height
    int8_t change;
    int8_t ending:1;
    int8_t panel;
};
*/
struct cpanel { // panel
    int16_t lc;
    int16_t ld;
    int16_t rc;
    int16_t rd;
    int8_t colour;
};

struct cpanels {
    int8_t num;
    struct cpanel ob[MAX_WALLS];
};

struct panel { // panel
    int16_t lp; // horizontal position
    int16_t lh; // vertical height
    int16_t rp; // horizontal position
    int16_t rh; // vertical height
    int8_t colour;
};

struct panels {
    int8_t num;
    struct panel ob[MAX_WALLS];
};

struct lpanel {
    int16_t lp;
    int16_t rp;
    int8_t colour;
};

struct lpanels {
    int8_t num;
    struct lpanel ob[MAX_WALLS];
};

struct crit_point { // panel (screen) poles
    int16_t p; // horizontal position
    int8_t panel_idx; // index into sorted panels->ob
    // each panel will start and stop exactly once, and the start will not be right of the stop
    //int8_t is_start; // 1 - is start, 0 - is end
};

struct crit_points {
    int8_t num;
    struct crit_point ob[MAX_WALLS * 2];
};

struct change {
    int16_t start;
    int8_t colour;
};

struct changes {
    int8_t num;
    struct change ob[MAX_WALLS];
};

int16_t mulsine(int16_t num, uint16_t ang);
int16_t mulcos(int16_t num, uint16_t ang);

// run once per frame
void wpoles_to_cpoles(struct wpoles *wpoles, struct camera *camera, struct cpoles *cpoles_out);          // rotate
void clip_cpoles(struct cpole *cpoles, struct cpole *cpoles_out);               // clip
void cpoles_to_cpanels(struct cpoles *cpoles, struct cpanels *cpanels_out);          // project into screen coordinates
struct cpanel cpanel_clip(struct cpanel cpanel);
void cpanels_to_panels(struct cpanels *cpanels, struct panels *panels_out);          // project into screen coordinates
void sort_panels_by_distance(struct panels *panels);

// obsolete
void cpoles_to_spoles(struct cpoles *cpoles, struct spoles *spoles_out);          // project into screen coordinates
void spoles_to_panels(struct spoles *spoles, struct panels *panels_out);          // filter out


// run once per scanline - this is time critical
void panels_to_crit_points(uint16_t line, struct panels *panels, struct crit_points *crit_points_out);
void sort_crit_points(struct crit_points *crit_points_io);               // sort
void crit_points_to_changes(struct crit_points *crit_points, struct panels *sorted_panels, struct changes *changes_out);

#endif
