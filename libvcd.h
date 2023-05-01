#ifndef LIBVCD_LIBVCD_H
#define LIBVCD_LIBVCD_H

#include <stdint.h>

#define LIBVCD_VERSION_SIZE 32
#define LIBVCD_DATE_SIZE 64
#define LIBVCD_TIMESCALE_UNIT_SIZE 4
#define LIBVCD_MAX_SIGNAL_COUNT 64
#define LIBVCD_MAX_VALUE_CHANGE_COUNT 2048
#define LIBVCD_TOKEN_SIZE 64

typedef uint32_t timestamp_t;

typedef struct {
    timestamp_t timestamp;
    char *value;
} value_change_t;

typedef struct {
    char *name;
    size_t size;
    value_change_t *value_changes;
} signal_t;

typedef struct {
    char *unit;
    size_t scale;
} timescale_t;

typedef struct {
    signal_t *signal_dumps;
    char *date;
    char *version;
    timescale_t timescale;
} vcd_t;

#endif //LIBVCD_LIBVCD_H
