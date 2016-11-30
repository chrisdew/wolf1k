#ifndef __DUAL_H
#define __DUAL_H

#include <stdint.h>

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

struct spole { // screen pole
    int16_t p; // horizontal position
    int16_t h; // vertical height
    int8_t colour;
    //struct spole *next;
    //struct spole *prev;
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

struct lpanel {
    int16_t lp;
    int16_t rp;
    int8_t colour;
};

struct lpole { // panel (screen) poles
    int16_t p; // horizontal position
    int16_t h; // vertical height
    int8_t start_of;
    int8_t end_of;
    int8_t colour;
};

struct rle { // run length encoding
    uint16_t start;
    uint8_t colour;
};

extern struct wpole wpoles[MAX_WALLS_PLUS_ONE];
extern struct camera camera;

// run once per frame
void wpoles_to_cpoles(struct wpole *wpoles, struct cpole *cpoles_out);          // rotate
void clip_cpoles(struct cpole *cpoles, struct cpole *cpoles_out);               // clip
void cpoles_to_spoles(struct cpole *cpoles, struct spole *spoles_out);          // project into screen coordinates
void spoles_to_panels(struct spole *spoles, struct panel *panels_out);          // filter out

// run once per scanline - this is time critical
void panels_to_lpoles(uint16_t line,struct panel *panels, struct lpole *lpoles_out);         // filter out
void sort_lpoles(struct lpole *lpoles_io);               // sort
void lpoles_to_rles(struct lpole *lpoles, struct rle *rles_out); // project

#endif
