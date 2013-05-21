#include <stdio.h>

#include <bondinfo/bondinfo.h>

int main(void) {
    bi_open_db("tripos.db");

    printf("C.1, C.1, 1: %lf\n", bi_bond_length(
        BI_ATOM_CARBON_SP1,
        BI_ATOM_CARBON_SP1,
        BI_BOND_SINGLE
    ));

    bi_close_db();

    return 0;
}
