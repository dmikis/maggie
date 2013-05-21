#include <stdlib.h>

#include <maggie/mol2.h>

#define UNUSED(x) ((void) (x))

void mol2_free(mol2_t * ptr) {
    mol2_molecule_free(ptr->molecule);
    free(ptr->atoms);
    free(ptr->bonds);
    free(ptr->substructures);
    free(ptr);
}

ssize_t mol2_atom_index_by_id(const mol2_t * m, uint32_t id) {
    ssize_t i = 0;

    while (i < m->molecule->atoms_num && m->atoms[i].id != id) {
        i += 1;
    }

    return i < m->molecule->atoms_num ? i : -1;
}
