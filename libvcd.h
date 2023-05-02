#ifndef LIBVCD_LIBVCD_H
#define LIBVCD_LIBVCD_H

#include <stdint.h>
#include <stdio.h>

#define LIBVCD_MAX_SIGNAL_COUNT 64
#define LIBVCD_MAX_VALUE_CHANGE_COUNT 2048

typedef uint32_t timestamp_t;

typedef struct {
    timestamp_t timestamp;
    char *value;
} value_change_t;

typedef struct {
    char *name;
    size_t size;
    value_change_t *value_changes;
    size_t changes_count;
} signal_t;

typedef struct {
    char *unit;
    size_t scale;
} timescale_t;

typedef struct {
    signal_t *signal_dumps;
    size_t signals_count;
    char *date;
    char *version;
    timescale_t timescale;
} vcd_t;

vcd_t *open_vcd(char *path);

#endif //LIBVCD_LIBVCD_H
