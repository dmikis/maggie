#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <maggie/mol2/molecule.h>

static int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr);
static void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr);

void mol2_molecule_free(mol2_molecule_t * molecule) {
    free(molecule->name);
    free(molecule->comment);
    free(molecule);
}

parser_t * mol2_molecule_parser_alloc(void) {
    void ** result_ptr = (void **) malloc(sizeof(void *));
    *result_ptr = malloc(sizeof(mol2_molecule_t));
    return parser_alloc(
        line_feed_cb,
        eof_feed_cb,
        free,
        result_ptr
    );
}

static int line_feed_cb(size_t lines_fed, const char * line, metadata_ptr_t metadata_ptr) {
    mol2_molecule_t * result = (mol2_molecule_t *) *((void **) metadata_ptr);

#   define RETURN_ERROR { \
        free(result); \
        *((void **) metadata_ptr) = NULL; \
        return 1; \
    }

    static char * types[] = {
        "SMALL",
        "BIOPOLYMER",
        "PROTEIN",
        "NUCLEIC_ACID",
        "SACCHARIDE"
    };

    static char * charge_types[] = {
        "NO_CHARGES",
        "DEL_RE",
        "GASTEIGER",
        "GAST_HUCK",
        "HUCKEL",
        "PULLMAN",
        "GAUSS80_CHARGES",
        "AMPAC_CHARGES",
        "MULLIKEN_CHARGES",
        "DICT_CHARGES",
        "MMFF94_CHARGES",
        "USER_CHARGES"
    };

    switch (lines_fed) {
        case 0: /* molname */
            result->name = (char *) malloc(strlen(line) + 1);
            strcpy(result->name, line);
            break;
        case 1: /* num_atoms [num_bonds [num_subst [num_feat [num_sets]]]] */
            result->bonds_num = 0;
            result->substructures_num = 0;
            result->features_num = 0;
            result->sets_num = 0;

            int fields_read = sscanf(line, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,
                                     &(result->atoms_num),
                                     &(result->bonds_num),
                                     &(result->substructures_num),
                                     &(result->features_num),
                                     &(result->sets_num)
                                     );

            if (fields_read < 1) RETURN_ERROR;

            break;
        case 2: /* mol_type */
            result->type = 0;

            while (result->type < 5 && strcmp(types[result->type], line)) {
                result->type += 1;
            }

            if (result->type == 5) RETURN_ERROR;

            break;
        case 3: /* charge_type */
            result->charge_type = 0;

            while (result->charge_type < 12 && strcmp(charge_types[result->charge_type], line)) {
                result->charge_type += 1;
            }

            if (result->type == 12) RETURN_ERROR;

            break;
        case 4: /* status_bits */
            /* ignored */
            break;
        case 5: /* mol_comment */
            result->comment = (char *) malloc(strlen(line));
            strcpy(result->comment, line);
            break;
        default:
            RETURN_ERROR;
    }
    return 0;
}

static void * eof_feed_cb(size_t lines_fed, metadata_ptr_t metadata_ptr) {
    return *((void **) metadata_ptr);
}

void mol2_molecule_serialize(const mol2_molecule_t * molecule, char ** buffer, size_t * buffer_size) {
    static char * types[] = {
        "SMALL",
        "BIOPOLYMER",
        "PROTEIN",
        "NUCLEIC_ACID",
        "SACCHARIDE"
    };

    static char * charge_types[] = {
        "NO_CHARGES",
        "DEL_RE",
        "GASTEIGER",
        "GAST_HUCK",
        "HUCKEL",
        "PULLMAN",
        "GAUSS80_CHARGES",
        "AMPAC_CHARGES",
        "MULLIKEN_CHARGES",
        "DICT_CHARGES",
        "MMFF94_CHARGES",
        "USER_CHARGES"
    };

    static char tmp_buffer[1024];

    sprintf(tmp_buffer, "%s\n%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 "\n%s\n%s\n\n%s",
        molecule->name,
        molecule->atoms_num,
        molecule->bonds_num,
        molecule->substructures_num,
        molecule->features_num,
        molecule->sets_num,
        types[molecule->type],
        charge_types[molecule->charge_type],
        molecule->comment
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
