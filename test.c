/*
 * This is the code which is compiled to run on a desktop machine.
 */

#include <stdio.h>
#include <stdint.h>

#include "dual.h"

void cpanel_print(char *prefix, struct cpanel cpanel) {
    printf("%s: panel(lc:%d,ld:%d,rc:%d,rd:%d,colour:%d)\n", prefix, cpanel.lc, cpanel.ld, cpanel.rc, cpanel.rd, cpanel.colour);
}

int main() {
    struct cpanel before;
    struct cpanel after;

    printf("\n");
    before = (struct cpanel) {.lc = -1000, .ld = 3000, .rc = 1000, .rd = 3000, .colour = 1};
    after = cpanel_clip(before);
    cpanel_print("before", before);
    cpanel_print(" after", after);

    printf("\n");
    before = (struct cpanel) {.lc = -1000, .ld = 1000, .rc = 1000, .rd = 1000, .colour = 1};
    after = cpanel_clip(before);
    cpanel_print("before", before);
    cpanel_print(" after", after);

    printf("\n");
    before = (struct cpanel) {.lc = -1000, .ld = 500, .rc = 1000, .rd = 500, .colour = 1};
    after = cpanel_clip(before);
    cpanel_print("before", before);
    cpanel_print(" after", after);

    printf("\n");
    before = (struct cpanel) {.lc = 250, .ld = 500, .rc = 250, .rd = -500, .colour = 1};
    after = cpanel_clip(before);
    cpanel_print("before", before);
    cpanel_print(" after", after);

    printf("\n");
    before = (struct cpanel) {.lc = -250, .ld = -500, .rc = -250, .rd = 500, .colour = 1};
    after = cpanel_clip(before);
    cpanel_print("before", before);
    cpanel_print(" after", after);

    return 0;
}
