#ifndef _MOL2_BOND_H_
#define _MOL2_BOND_H_

#include <maggie/parser.h>

typedef struct {
    uint32_t id;
    uint32_t origin_atom_id;
    uint32_t target_atom_id;
    uint8_t type;
} mol2_bond_t;

parser_t * mol2_bond_parser_alloc(size_t bonds_num);

void mol2_bond_serialize(const mol2_bond_t * bond, char ** buffer, size_t * buffer_size);

#endif // _MOL2_BOND_H_
