#ifndef _MOL2_H_
#define _MOL2_H_

#include <unistd.h>

#include "mol2/molecule.h"
#include "mol2/atom.h"
#include "mol2/bond.h"
#include "mol2/substructure.h"

typedef struct {
    mol2_molecule_t * molecule;
    mol2_atom_t * atoms;
    mol2_bond_t * bonds;
    mol2_substructure_t * substructures;
} mol2_t;

void mol2_free(mol2_t * ptr);

ssize_t mol2_atom_index_by_id(const mol2_t * m, uint32_t id);

#endif // _MOL2_H_
