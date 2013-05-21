#ifndef _MOL2_ATOM_H_
#define _MOL2_ATOM_H_

#include <stdint.h>
#include <stdlib.h>

#include <maggie/parser.h>

typedef struct {
    uint32_t id;
    char * name;
    double x, y, z;
    char * sybyl_type;
    int32_t substructure_id;
    char * substructure_name;
    float charge;
} mol2_atom_t;

void mol2_atom_free(mol2_atom_t * ptr);

parser_t * mol2_atom_parser_alloc(size_t atoms_num);

void mol2_atom_serialize(const mol2_atom_t * atom, char ** buffer, size_t * buffer_size);

#endif // _MOL2_ATOM_H_
