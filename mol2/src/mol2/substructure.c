#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <maggie/mol2/substructure.h>

#define UNUSED(x) ((void) (x))

typedef struct {
    mol2_substructure_t * substructures;
    size_t substructures_num;
} metadata_t;

static int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr);
static void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr);

parser_t * mol2_substructure_parser_alloc(size_t substructures_num) {
    metadata_t * metadata = (metadata_t *) malloc(sizeof(metadata_t));

    metadata->substructures = (mol2_substructure_t *) malloc(substructures_num * sizeof(mol2_substructure_t));
    metadata->substructures_num = substructures_num;

    return parser_alloc(
        line_feed_cb,
        eof_feed_cb,
        free,
        (void *) metadata
    );
}

static int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr) {
    metadata_t * metadata = (metadata_t *) metadata_ptr;

#   define RETURN_ERROR { \
        free(metadata->substructures); \
        metadata->substructures = NULL; \
        return 1; \
    }

    if (lines_fed >= metadata->substructures_num) RETURN_ERROR;

    mol2_substructure_t * curr_struct = metadata->substructures + lines_fed;

    curr_struct->type = 0;

    char name_buffer[32];
    char type_buffer[32] = "";
    int fields_read = sscanf(line, "%" SCNu32 " %s %" SCNu32 " %s",
        &(curr_struct->id),
        name_buffer,
        &(curr_struct->root_atom),
        type_buffer
    );

    if (fields_read < 3) RETURN_ERROR;

    curr_struct->name = (char *) malloc(strlen(name_buffer) + 1);
    strcpy(curr_struct->name, name_buffer);

    static char * substructure_types[] = { "temp", "perm", "residue", "group", "domain" };
    while (curr_struct->type < 5 && strcmp(substructure_types[curr_struct->type], type_buffer)) {
        curr_struct->type += 1;
    }

    return 0;
}

static void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr) {
    UNUSED(lines_fed);

    return (void *) ((metadata_t *) metadata_ptr)->substructures;
}
