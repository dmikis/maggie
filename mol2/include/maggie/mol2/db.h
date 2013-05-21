#ifndef _MOL2_DB_H_
#define _MOL2_DB_H_

#include <maggie/parser.h>

#include "../mol2.h"

typedef struct {
    mol2_t * rec;
    void * next;
} mol2_db_rec_t;

typedef mol2_db_rec_t * mol2_db_t;

void mol2_db_free(mol2_db_t db);

#define MOL2_DB_NEXT(db) ((mol2_db_t) (db)->next)

parser_t * mol2_db_parser_alloc(void);

#endif // _MOL2_DB_H_
