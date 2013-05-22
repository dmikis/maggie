#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <maggie/mol2/atom.h>

#define UNUSED(x) ((void) (x))

typedef struct {
    mol2_atom_t * atoms;
    size_t atoms_num;
} metadata_t;

static int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr);
static void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr);

void mol2_atom_free(mol2_atom_t * ptr) {
    free(ptr->name);
    free(ptr->sybyl_type);
    free(ptr->substructure_name);
    free(ptr);
}

mgg_parser_t * mol2_atom_parser_alloc(size_t atoms_num) {
    metadata_t * metadata = (metadata_t *) malloc(sizeof(metadata_t));

    metadata->atoms = (mol2_atom_t *) malloc(atoms_num * sizeof(mol2_atom_t));
    metadata->atoms_num = atoms_num;

    return mgg_parser_alloc(
        line_feed_cb,
        eof_feed_cb,
        free,
        (metadata_ptr_t) metadata
    );
}

int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr) {
    metadata_t * metadata = (metadata_t *) metadata_ptr;

#   define RETURN_ERROR { \
        free(metadata->atoms); \
        metadata->atoms = NULL; \
    }

    if (lines_fed >= metadata->atoms_num) RETURN_ERROR;

    mol2_atom_t * current_atom = metadata->atoms + lines_fed;

    current_atom->substructure_id = -1;
    current_atom->substructure_name = NULL;
    current_atom->charge = nanf(NULL);

    char name_buffer[8];
    char sybyl_type_buffer[8];
    char substructure_name_buffer[256];
    int fields_read = sscanf(line, "%" SCNu32 " %s %lf %lf %lf %s %" SCNd32 " %s %f",
        &(current_atom->id),
        name_buffer,
        &(current_atom->x),
        &(current_atom->y),
        &(current_atom->z),
        sybyl_type_buffer,
        &(current_atom->substructure_id),
        substructure_name_buffer,
        &(current_atom->charge)
    );

    if (fields_read < 6) RETURN_ERROR;

#   define STR_FIELD_ALLOC_AND_COPY(f) { \
        current_atom->f = (char *) malloc(strlen(f ## _buffer) + 1); \
        strcpy(current_atom->f, f ## _buffer); \
    }

    STR_FIELD_ALLOC_AND_COPY(name);
    STR_FIELD_ALLOC_AND_COPY(sybyl_type);
    STR_FIELD_ALLOC_AND_COPY(substructure_name);

    return 0;
}

void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr) {
    UNUSED(lines_fed);

    return (void *) ((metadata_t *) metadata_ptr)->atoms;
}

void mol2_atom_serialize(const mol2_atom_t * atom, char ** buffer, size_t * buffer_size) {
    static char tmp_buffer[1024];

    sprintf(tmp_buffer, "%" SCNu32 " %s %lf %lf %lf %s %" SCNd32 " %s %f",
        atom->id,
        atom->name,
        atom->x,
        atom->y,
        atom->z,
        atom->sybyl_type,
        atom->substructure_id,
        atom->substructure_name,
        atom->charge
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
