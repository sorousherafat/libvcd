#include "../libvcd.h"
#include <stdio.h>
#include <stdlib.h>

void print_vcd(vcd_t *vcd) {
    printf("{\n\tdate=\"%s\",\n\tversion=\"%s\",\n\ttimescale= {\n\t\tunit=\"%s\",\n\t\tscale=\"%zu\"\n\t},\n\tsignal= {\n", vcd->date, vcd->version,
           vcd->timescale.unit, vcd->timescale.scale);
    for (int i = 0; i < vcd->signals_count; ++i) {
        printf("\t\t%s= {\n\t\t\tsize=%zu,\n\t\t\tchanges= {\n", vcd->signal_dumps[i].name, vcd->signal_dumps[i].size);
        for (int j = 0; j < vcd->signal_dumps[i].changes_count; ++j) {
            printf("\t\t\t\t{\n\t\t\t\t\ttimestamp=%u,\n\t\t\t\t\tvalue=%s\n\t\t\t\t},\n", vcd->signal_dumps[i].value_changes[j].timestamp, vcd->signal_dumps[i].value_changes[j].value);
        }
        printf("\t\t\t},\n\t\t},\n");
    }
    printf("\t}\n}\n");
}

int main(int argc, char *argv[]) {
    if (argc % 2 == 1) {
        fprintf(stderr, "Usage: test <vcd-file> [signal-name timestamp] ...\n");
        exit(EXIT_FAILURE);
    }

    vcd_t *vcd = open_vcd(argv[1]);
    print_vcd(vcd);
    printf("\n");

    for (int i = 2; i < argc; i += 2) {
        timestamp_t timestamp = strtol(argv[i + 1], NULL, 0);
        printf("%s at %u equals %s\n", argv[i], timestamp, get_value_from_vcd(vcd, argv[i], timestamp));
    }
}
