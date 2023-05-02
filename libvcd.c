#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include "libvcd.h"

#define isexpression(c) (strchr("-0123456789zZxXbU", (c)))

static void init_vcd(vcd_t **vcd);

static void parse_instruction(FILE *file, vcd_t *vcd);

static void parse_timestamp(FILE *file, vcd_t *vcd, timestamp_t *timestamp);

static void parse_assignment(FILE *file, vcd_t *vcd, timestamp_t timestamp);

static size_t get_signal_index(const char *string);

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

void init_vcd(vcd_t **vcd) {
    *vcd = (vcd_t *) malloc(sizeof(vcd_t));
    (*vcd)->signal_dumps = (signal_t *) malloc(LIBVCD_MAX_SIGNAL_COUNT * sizeof(signal_t));
    (*vcd)->signals_count = 0;
}

void parse_instruction(FILE *file, vcd_t *vcd) {
    char *instruction;
    if (fscanf(file, "%ms", &instruction) != 1)
        return;

    if (strcmp(instruction, "end") == 0 || strcmp(instruction, "dumpvars") == 0)
        return;

    if (strcmp(instruction, "scope") == 0 || strcmp(instruction, "upscope") == 0 ||
        strcmp(instruction, "enddefinitions") == 0) {
        fscanf(file, "\n%*[^$]");
        return;
    }

    if (strcmp(instruction, "var") == 0) {
        signal_t signal;
        signal.changes_count = 0;
        signal.value_changes = (value_change_t *) malloc(LIBVCD_MAX_VALUE_CHANGE_COUNT * sizeof(value_change_t));
        char *signal_id;
        fscanf(file, " %*s %zu %m[^ ] %m[^ $]%*[^$]", &signal.size, &signal_id, &signal.name);
        size_t index = get_signal_index(signal_id);
        if (vcd->signal_dumps[index].size == 0)
            vcd->signals_count += 1;
        vcd->signal_dumps[index] = signal;
        return;
    }

    if (strcmp(instruction, "date") == 0) {
        fscanf(file, "\n%m[^$\n]", &vcd->date);
        return;
    }

    if (strcmp(instruction, "version") == 0) {
        fscanf(file, "\n%m[^$\n]", &vcd->version);
        return;
    }

    if (strcmp(instruction, "timescale") == 0) {
        fscanf(file, "\n\t%zu%m[^$\n]", &vcd->timescale.scale, &vcd->timescale.unit);
        return;
    }

    fprintf(stderr, "Unknown token: '%s'", instruction);
}

void parse_timestamp(FILE *file, vcd_t *vcd, timestamp_t *timestamp) {
    fscanf(file, "%u", timestamp);
}

void parse_assignment(FILE *file, vcd_t *vcd, timestamp_t timestamp) {
    char *buffer;
    char *value;
    char *signal_id;
    fscanf(file, "%m[^\n]", &buffer);

    if (strchr("01xXzZ", buffer[0])) {
        value = (char *) malloc(2 * sizeof(char));
        sscanf(buffer, "%1s%m[^\n]", value, &signal_id);
    } else {
        sscanf(buffer, "%m[^ ] %m[^\n]", &value, &signal_id);
    }

    size_t index = get_signal_index(signal_id);
    size_t changes_count = vcd->signal_dumps[index].changes_count;
    vcd->signal_dumps[index].value_changes[changes_count].timestamp = timestamp;
    vcd->signal_dumps[index].value_changes[changes_count].value = value;
    vcd->signal_dumps[index].changes_count += 1;
}

size_t get_signal_index(const char *string) {
    size_t id = *string - '!';
    if (id > LIBVCD_MAX_SIGNAL_COUNT) {
        fprintf(stderr, "Signal count exceeded the maximum allowed number");
        exit(EXIT_FAILURE);
    }

    return id;
}
