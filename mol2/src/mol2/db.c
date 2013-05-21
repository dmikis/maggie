#include <string.h>

#include <maggie/mol2/db.h>

#define UNUSED(x) ((void) (x))

typedef struct {
    parser_t * current_parser;
    mol2_t * current_rec;
    uint8_t current_rec_type;
    mol2_db_t db;
} metadata_t;

static mol2_db_t mol2_db_push(
    mol2_db_t db,
    mol2_t * rec
);

static int line_feed_cb(
    size_t lines_fed,
    const char * line,
    metadata_ptr_t metadata_ptr
);

static void * eof_feed_cb(
    size_t lines_fed,
    metadata_ptr_t metadata_ptr
);

static metadata_ptr_t metadata_alloc(
    void
);

static void metadata_free(
    metadata_ptr_t ptr
);

parser_t * mol2_db_parser_alloc(void) {
    return parser_alloc(
        line_feed_cb,
        eof_feed_cb,
        metadata_free,
        metadata_alloc()
    );
}

mol2_db_t mol2_db_push(
    mol2_db_t db,
    mol2_t * rec
) {
    mol2_db_t new_head = (mol2_db_t) malloc(sizeof(mol2_db_rec_t));
    new_head->rec = rec;
    new_head->next = (void *) db;
    return new_head;
}

void mol2_db_free(
    mol2_db_t db
) {
    mol2_db_rec_t * current_rec = db;

    while (current_rec) {
        mol2_db_rec_t * next = MOL2_DB_NEXT(current_rec);
        mol2_free(current_rec->rec);
        free(current_rec);
        current_rec = next;
    }
}

int line_feed_cb(
    size_t lines_fed,
    const char * line,
    metadata_ptr_t metadata_ptr
) {
    UNUSED(lines_fed);

#   define RETURN_ERROR { \
        mol2_db_free(metadata->db); \
        metadata->db = NULL; \
        return 1; \
    }

    metadata_t * metadata = (metadata_t *) metadata_ptr;

    static char * rec_types[] = {
        "@<TRIPOS>MOLECULE",
        "@<TRIPOS>ATOM",
        "@<TRIPOS>BOND",
        "@<TRIPOS>SUBSTRUCTURE"
    };

#   define MOLECULE_REC 0
#   define ATOM_REC     1
#   define BOND_REC     2
#   define SUBSTRUCTURE_REC 3

    if (line[0] == '@') {
        if (metadata->current_parser) {
            void * parser_result = parser_feed_eof(metadata->current_parser);
            parser_free(metadata->current_parser);
            metadata->current_parser = NULL;

            if (parser_result) {
                switch (metadata->current_rec_type) {
                    case MOLECULE_REC:
                        metadata->current_rec->molecule = (mol2_molecule_t *) parser_result;
                        break;
                    case ATOM_REC:
                        metadata->current_rec->atoms = (mol2_atom_t *) parser_result;
                        break;
                    case BOND_REC:
                        metadata->current_rec->bonds = (mol2_bond_t *) parser_result;
                        break;
                    case SUBSTRUCTURE_REC:
                        metadata->current_rec->substructures = (mol2_substructure_t *) parser_result;
                        break;
                }
            } else RETURN_ERROR;
        }

        int rec_type = 0;

        while (rec_type < 4 && strcmp(rec_types[rec_type], line)) {
            rec_type += 1;
        }

        if (rec_type == 4) RETURN_ERROR;

        metadata->current_rec_type = rec_type;

        switch (rec_type) {
            case MOLECULE_REC:
                if (metadata->current_rec) {
                    metadata->db = mol2_db_push(metadata->db, metadata->current_rec);
                }

                metadata->current_rec = (mol2_t *) malloc(sizeof(mol2_t));
                metadata->current_parser = mol2_molecule_parser_alloc();

                break;

            case ATOM_REC:
                if (!metadata->current_rec) RETURN_ERROR;

                metadata->current_parser =
                    mol2_atom_parser_alloc(metadata->current_rec->molecule->atoms_num);

                break;

            case BOND_REC:
                if (!metadata->current_rec) RETURN_ERROR;

                metadata->current_parser =
                    mol2_bond_parser_alloc(metadata->current_rec->molecule->bonds_num);

                break;

            case SUBSTRUCTURE_REC:
                if (!metadata->current_rec) RETURN_ERROR;

                metadata->current_parser =
                    mol2_substructure_parser_alloc(metadata->current_rec->molecule->substructures_num);

                break;
        }
    } else {
        if (!metadata->current_parser) RETURN_ERROR;

        /* TODO comments support */
        return parser_feed_line(metadata->current_parser, line);
    }

    return 0;
}

void * eof_feed_cb(
    size_t lines_fed,
    metadata_ptr_t metadata_ptr
) {
    metadata_t * metadata = (metadata_t *) metadata_ptr;

    if (metadata->current_parser) {
        void * parser_result = parser_feed_eof(metadata->current_parser);
        parser_free(metadata->current_parser);
        metadata->current_parser = NULL;

        if (parser_result) {
            switch (metadata->current_rec_type) {
                case MOLECULE_REC:
                    metadata->current_rec->molecule = (mol2_molecule_t *) parser_result;
                    break;
                case ATOM_REC:
                    metadata->current_rec->atoms = (mol2_atom_t *) parser_result;
                    break;
                case BOND_REC:
                    metadata->current_rec->bonds = (mol2_bond_t *) parser_result;
                    break;
                case SUBSTRUCTURE_REC:
                    metadata->current_rec->substructures = (mol2_substructure_t *) parser_result;
                    break;
            }
        } else {
            mol2_db_free(metadata->db);
            return NULL;
        }
    }

    return mol2_db_push(metadata->db, metadata->current_rec);
}

metadata_ptr_t metadata_alloc(
    void
) {
    metadata_t * metadata = (metadata_t *) malloc(sizeof(metadata_t));

    metadata->current_parser = NULL;
    metadata->current_rec = NULL;
    metadata->db = NULL;

    return (metadata_ptr_t) metadata;
}

void metadata_free(
    metadata_ptr_t ptr
) {
    metadata_t * metadata = (metadata_t *) ptr;

    if (metadata->current_parser) {
        parser_free(metadata->current_parser);
    }

    free(ptr);
}
