#ifndef _MOL2_SUBSTRUCTURE_H_
#define _MOL2_SUBSTRUCTURE_H_

#include <stdint.h>
#include <stdlib.h>

#include <maggie/parser.h>

typedef struct {
    uint32_t id;
    char * name;
    uint32_t root_atom;
    uint8_t type;
    /* FIXME add other fields */
} mol2_substructure_t;

parser_t * mol2_substructure_parser_alloc(size_t substructures_num);

#endif // _MOL2_SUBSTRUTURE_H_
