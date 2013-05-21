#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <maggie/mol2/bond.h>

#define UNUSED(x) ((void) (x))

typedef struct {
    mol2_bond_t * bonds;
    size_t bonds_num;
} metadata_t;

static int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr);
static void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr);

parser_t * mol2_bond_parser_alloc(size_t bonds_num) {
    metadata_t * metadata = (metadata_t *) malloc(sizeof(metadata_t));

    metadata->bonds = (mol2_bond_t *) malloc(bonds_num * sizeof(mol2_bond_t));
    metadata->bonds_num = bonds_num;

    return parser_alloc(
        line_feed_cb,
        eof_feed_cb,
        free,
        metadata
    );
}

int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr) {
    metadata_t * metadata = (metadata_t *) metadata_ptr;

#   define RETURN_ERROR { \
        free(metadata->bonds); \
        metadata->bonds = NULL; \
        return 1; \
    }

    if (metadata->bonds_num <= lines_fed) RETURN_ERROR;

    mol2_bond_t * curr_bond = metadata->bonds + lines_fed;

    char type_buffer[8];
    int fields_read = sscanf(line, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %s",
        &(curr_bond->id),
        &(curr_bond->origin_atom_id),
        &(curr_bond->target_atom_id),
        type_buffer
    );

    if (fields_read < 4) RETURN_ERROR;

    static char * bond_types[] = { "1", "2", "3", "ar", "am", "du", "un", "nc" };
    curr_bond->type = 0;

    while (curr_bond->type < 8 && strcmp(bond_types[curr_bond->type], type_buffer)) {
        curr_bond->type += 1;
    }

    if (curr_bond->type == 8) RETURN_ERROR;

    return 0;
}

void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr) {
    UNUSED(lines_fed);

    return (void *) ((metadata_t *) metadata_ptr)->bonds;
}

void mol2_bond_serialize(const mol2_bond_t * bond, char ** buffer, size_t * buffer_size) {
    static char * bond_types[] = { "1", "2", "3", "ar", "am", "du", "un", "nc" };
    static char tmp_buffer[1024];

    sprintf(tmp_buffer, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %s",
        bond->id,
        bond->origin_atom_id,
        bond->target_atom_id,
        bond_types[bond->type]
    );

    size_t tmp_buffer_len = strlen(tmp_buffer);

    if (*buffer && tmp_buffer_len >= *buffer_size) {
        free(*buffer);
        *buffer = NULL;
    }

    if (!*buffer) {
        *buffer_size = tmp_buffer_len + 1;
        *buffer = (char *) calloc(*buffer_size, 1);
    }

    strcpy(*buffer, tmp_buffer);
}
