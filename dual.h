#ifndef __DUAL_H
#define __DUAL_H

#include <stdint.h>
#include <stdio.h>

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
    int8_t colour;
    int8_t ending:1;
    int8_t panel;
};
*/

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
    int8_t is_start; // 1 - is start, 0 - is end
};

struct crit_points {
    int8_t num;
    struct crit_point ob[MAX_WALLS * 2];
};

struct rle {
    uint16_t start;
    uint8_t colour;
};

struct colour {
    uint16_t start;
    uint8_t colour;
};

struct colours {
    int8_t num;
    struct colour ob[MAX_WALLS];
};

extern struct wpole wpoles[MAX_WALLS_PLUS_ONE];
extern struct camera camera;

// run once per frame
void wpoles_to_cpoles(struct wpole *wpoles, struct cpole *cpoles_out);          // rotate
void clip_cpoles(struct cpole *cpoles, struct cpole *cpoles_out);               // clip
void cpoles_to_spoles(struct cpoles *cpoles, struct spoles *spoles_out);          // project into screen coordinates
void spoles_to_panels(struct spoles *spoles, struct panels *panels_out);          // filter out
void sort_panels_by_distance(struct panels *panels);

// run once per scanline - this is time critical
void panels_to_crit_points(uint16_t line, struct panels *panels, struct crit_points *crit_points_out);
void sort_crit_points(struct crit_points *crit_points_io);               // sort
void crit_points_to_colours(struct crit_points *crit_points, struct colours *colours_out); // project

#endif
