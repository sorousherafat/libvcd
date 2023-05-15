#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "libvcd.h"

#define isexpression(c) (strchr("-0123456789zZxXbU", (c)))

static void init_vcd(vcd_t **vcd);

static void parse_instruction(FILE *file, vcd_t *vcd);

static void parse_timestamp(FILE *file, __attribute__((unused)) vcd_t *vcd, timestamp_t *timestamp);

static void parse_assignment(FILE *file, vcd_t *vcd, timestamp_t timestamp);

static size_t get_signal_index(const char *string);

static signal_t *get_signal_by_name(vcd_t *vcd, char *name);

static char *get_signal_value(signal_t *signal, timestamp_t timestamp);

vcd_t *open_vcd(char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    vcd_t *vcd;
    init_vcd(&vcd);
    timestamp_t current_timestamp = 0;

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '$') {
            parse_instruction(file, vcd);
            continue;
        }

        if (c == '#') {
            parse_timestamp(file, vcd, &current_timestamp);
            continue;
        }

        if (isexpression(c)) {
            ungetc(c, file);
            parse_assignment(file, vcd, current_timestamp);
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

    return vcd;
}

char *get_value_from_vcd(vcd_t *vcd, char *signal_name, timestamp_t timestamp) {
    signal_t *signal = get_signal_by_name(vcd, signal_name);
    if (signal == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return get_signal_value(signal, timestamp);
}

void init_vcd(vcd_t **vcd) {
    *vcd = (vcd_t *) calloc(1, sizeof(vcd_t));
}

void parse_instruction(FILE *file, vcd_t *vcd) {
    char instruction[512];
    if (fscanf(file, "%s", instruction) != 1)
        return;

    if (strcmp(instruction, "end") == 0 || strcmp(instruction, "dumpvars") == 0)
        return;

    if (strcmp(instruction, "scope") == 0 || strcmp(instruction, "upscope") == 0 ||
        strcmp(instruction, "enddefinitions") == 0) {
        fscanf(file, "\n%*[^$]");
        return;
    }

    if (strcmp(instruction, "var") == 0) {
        signal_t *signal = &vcd->signals[vcd->signals_count];
        vcd->signals_count += 1;

        char signal_id[8];
        fscanf(file, " %*s %zu %[^ ] %[^ $]%*[^$]", &signal->size, signal_id, signal->name);
        size_t index = get_signal_index(signal_id);

        if (vcd->signals[index].size != 0) {
            return;
        }

        return;
    }

    if (strcmp(instruction, "date") == 0) {
        fscanf(file, "\n%[^$\n]", vcd->date);
        return;
    }

    if (strcmp(instruction, "version") == 0) {
        fscanf(file, "\n%[^$\n]", vcd->version);
        return;
    }

    if (strcmp(instruction, "timescale") == 0) {
        fscanf(file, "\n\t%zu%[^$\n]", &vcd->timescale.scale, vcd->timescale.unit);
        return;
    }

    fprintf(stderr, "Unknown token: '%s'", instruction);
}

void parse_timestamp(FILE *file, __attribute__((unused)) vcd_t *vcd, timestamp_t *timestamp) {
    fscanf(file, "%u", timestamp);
}

void parse_assignment(FILE *file, vcd_t *vcd, timestamp_t timestamp) {
    char buffer[128];
    char value[64];
    char signal_id[8];
    fscanf(file, "%[^\n]", buffer);

    if (strchr("01xXzZ", buffer[0])) {
        sscanf(buffer, "%1s%[^\n]", value, signal_id);
    } else {
        sscanf(buffer, "%[^ ] %[^\n]", value, signal_id);
    }

    size_t index = get_signal_index(signal_id);
    size_t changes_count = vcd->signals[index].changes_count;
    vcd->signals[index].value_changes[changes_count].timestamp = timestamp;
    strncpy(vcd->signals[index].value_changes[changes_count].value, value, LIBVCD_SIGNAL_SIZE);
    vcd->signals[index].changes_count += 1;
}

size_t get_signal_index(const char *string) {
    size_t id = *string - '!';
    if (id >= LIBVCD_SCOPE_SIGNAL_COUNT) {
        fprintf(stderr, "Signal count exceeded the maximum allowed number");
        exit(EXIT_FAILURE);
    }

    return id;
}

signal_t *get_signal_by_name(vcd_t *vcd, char *name) {
    for (int i = 0; i < vcd->signals_count; ++i) {
        if (strcmp(vcd->signals[i].name, name) == 0)
            return &vcd->signals[i];
    }

    return NULL;
}

char *get_signal_value(signal_t *signal, timestamp_t timestamp) {
    char *previous_value;
    for (int i = 0; i < signal->changes_count; ++i) {
        value_change_t *value_change = &signal->value_changes[i];
        if (timestamp < value_change->timestamp)
            break;
        previous_value = value_change->value;
    }
    return previous_value;
}
