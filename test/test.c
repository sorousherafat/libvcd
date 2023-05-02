#include "../libvcd.h"
#include <stdio.h>
#include <stdlib.h>

void print_vcd(vcd_t *vcd) {
    printf("{\n\tdate=\"%s\",\n\tversion=\"%s\",\n\ttimescale=\"{\n\t\tunit=\"%s\",\n\t\tscale=\"%zu\"\n\t}\"\n}", vcd->date, vcd->version,
           vcd->timescale.unit, vcd->timescale.scale);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: test <vcd-file>");
        exit(EXIT_FAILURE);
    }

    vcd_t *vcd = open_vcd(argv[1]);
    print_vcd(vcd);
}
