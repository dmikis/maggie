#ifndef _MOL2_MOLECULE_H_
#define _MOL2_MOLECULE_H_

#include <stdint.h>

#include <maggie/parser.h>

typedef struct {
    char * name;
    uint32_t atoms_num;
    uint32_t bonds_num;
    uint32_t substructures_num;
    uint32_t features_num;
    uint32_t sets_num;
    uint8_t type;
    uint8_t charge_type;
    char * comment;
} mol2_molecule_t;

void mol2_molecule_free(mol2_molecule_t * molecule);

parser_t * mol2_molecule_parser_alloc(void);

void mol2_molecule_serialize(const mol2_molecule_t * molecule, char ** buffer, size_t * buffer_size);

#endif // _MOL2_MOLECULE_H_
