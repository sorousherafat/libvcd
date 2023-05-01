#ifndef LIBVCD_LIBVCD_H
#define LIBVCD_LIBVCD_H

#include <stdint.h>

typedef uint64_t timestamp_t;

typedef struct {
    timestamp_t timestamp;
    char *value;
} value_change_t;

typedef struct {
    char *name;
    value_change_t *value_changes;
} signal_vcd_t;

typedef struct {
    char *unit;
    size_t scale;
} timescale_t;

typedef struct {
    signal_vcd_t *signal_dumps;
    char *date;
    char *version;
    timescale_t timescale;
} vcd_t;

#endif //LIBVCD_LIBVCD_H
