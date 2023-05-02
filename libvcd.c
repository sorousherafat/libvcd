#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "libvcd.h"

#define isexpression(c) (strchr("-0123456789zZxXbU", (c)))

static void init_vcd(vcd_t **vcd);
static void parse_var(FILE *file, vcd_t *vcd);
static void parse_timestamp(FILE *file, vcd_t *vcd);
static void parse_assignment(FILE *file, vcd_t *vcd);

vcd_t *open_vcd(char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    vcd_t *vcd;
    init_vcd(&vcd);

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '$') {
            parse_var(file, vcd);
            continue;
        }

        if (c == '#') {
            parse_timestamp(file, vcd);
            continue;
        }

        if (isexpression(c)) {
            ungetc(c, file);
            parse_assignment(file, vcd);
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

void init_vcd(vcd_t **vcd) {
    *vcd = (vcd_t *) malloc(sizeof(vcd_t));
    (*vcd)->version = (char *) malloc(LIBVCD_VERSION_SIZE);
    (*vcd)->date = (char *) malloc(LIBVCD_DATE_SIZE);
    (*vcd)->timescale.unit = (char *) malloc(LIBVCD_TIMESCALE_UNIT_SIZE);
    (*vcd)->signal_dumps = (signal_t *) malloc(LIBVCD_MAX_SIGNAL_COUNT * sizeof(signal_t));
}

void parse_var(FILE *file, vcd_t *vcd) {

}

void parse_timestamp(FILE *file, vcd_t *vcd) {

}

void parse_assignment(FILE *file, vcd_t *vcd) {

}
