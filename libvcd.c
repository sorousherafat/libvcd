#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "libvcd.h"

#define isexpression(c) (strchr("-0123456789zZxXbU", (c)))

static FILE *file;
static vcd_t *vcd;

static void init_vcd(void);
static void parse_var(void);
static void parse_timestamp(void);
static void parse_assignment(void);

vcd_t *open_vcd(char *path) {
    file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    init_vcd();

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '$') {
            parse_var();
            continue;
        }

        if (c == '#') {
            parse_timestamp();
            continue;
        }

        if (isexpression(c)) {
            ungetc(c, file);
            parse_assignment();
            continue;
        }

        if (isspace(c))
            continue;

#ifdef LIBVCD_VERBOSE
        fprintf(stderr, "invalid character at the beginning of the line: '%c'\n", c);
#endif
        errno = EINVAL;
        return NULL;
    }
}

void init_vcd() {
    vcd = (vcd_t *) malloc(sizeof(vcd_t));
    vcd->version = (char *) malloc(LIBVCD_VERSION_SIZE);
    vcd->date = (char *) malloc(LIBVCD_DATE_SIZE);
    vcd->timescale.unit = (char *) malloc(LIBVCD_TIMESCALE_UNIT_SIZE);
    vcd->signal_dumps = (signal_t *) malloc(LIBVCD_MAX_SIGNAL_COUNT * sizeof(signal_t));
}

void parse_var(void) {

}

void parse_timestamp(void) {

}

void parse_assignment(void) {

}
