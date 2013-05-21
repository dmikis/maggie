#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>

#include <maggie/bondinfo.h>

#include <maggie/mol2.h>
#include <maggie/mol2/db.h>

#include <maggie/parser.h>

#include <maggie/dg.h>

#include "dlimits.h"
#include "errfn.h"
#include "util.h"

int main(int argc, char ** argv) {
    long long x;
    asm("rdtsc" : "=A"(x));
    srand(x);

    if (argc < 2) LOG_FATAL("File cannot be omitted");

    FILE * input_file = fopen(argv[1], "r");

    if (!input_file) LOG_FATAL("Cannot open %s", argv[1]);

    parser_t * db_parser = mol2_db_parser_alloc();

    char * line_buffer = NULL;
    size_t line_buff_size = 0l;
    while (getline(&line_buffer, &line_buff_size, input_file) > 0) {
        size_t line_buff_len = strlen(line_buffer);
        while (isspace(line_buffer[line_buff_len - 1])) {
            line_buffer[line_buff_len-- - 1] = '\0';
        }
        parser_feed_line(db_parser, line_buffer);
    }

    mol2_db_t db = parser_feed_eof(db_parser);
    parser_free(db_parser);

    fclose(input_file);

    if (!db) LOG_FATAL("Database parsing failed");

    /*bi_open_db("/Users/dmikis/Documents/Coding/lab/bondinfo/data/cyclohexan.db");*/
    bi_open_db("/Users/dmikis/Documents/Coding/lab/bondinfo/data/tripos.db");

    do {
        mol2_t * mol = db->rec;
        size_t N = mol->molecule->atoms_num;
        gsl_matrix * dist_limits = gsl_matrix_calloc(N, N);

        set_default_ulimits(dist_limits);
#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("After default uppper limit:", dist_limits);
#       endif

        set_default_llimits(dist_limits, mol);
#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("After default lower limit:", dist_limits);
#       endif

        set_bond_lengths(dist_limits, mol);
#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("After bond lengths:", dist_limits);
#       endif

        set_angle_limits(dist_limits, mol);
#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("After bond angles:", dist_limits);
#       endif

        apply_triangle_inequality(dist_limits);
#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("After triangle rule:", dist_limits);
#       endif

        gsl_matrix * distances = gsl_matrix_calloc(N, N);

        for (size_t i = 0; i < N; ++i) {
            for (size_t j = i + 1; j < N; ++j) {
                double ulimit = gsl_matrix_get(dist_limits, i, j);
                double llimit = gsl_matrix_get(dist_limits, j, i);
                double d = llimit + (ulimit - llimit) * rand() / RAND_MAX;

                gsl_matrix_set(distances, i, j, d);
                gsl_matrix_set(distances, j, i, d);
            }
        }

#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("Random generated distance matrix:", distances);
#       endif

        gsl_matrix * atoms_coords = dists_to_coords(distances);
        gsl_vector * atoms_coords_v = gsl_vector_alloc(3 * mol->molecule->atoms_num);

#       if defined(DEBUG) && (LOG_MASK & LOG_MATRICES)
        PRINT_MATRIX("After dg:", coords_to_dists(atoms_coords));
#       endif

        for (size_t i = 0; i < mol->molecule->atoms_num; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                gsl_vector_set(atoms_coords_v, 3 * i + j, gsl_matrix_get(atoms_coords, i, j));
            }
        }

        gsl_vector * refined_coords = refine_coords(atoms_coords_v, dist_limits);

        for (size_t i = 0; i < mol->molecule->atoms_num; ++i) {
            mol->atoms[i].x = gsl_vector_get(refined_coords, 3 * i);
            mol->atoms[i].y = gsl_vector_get(refined_coords, 3 * i + 1);
            mol->atoms[i].z = gsl_vector_get(refined_coords, 3 * i + 2);
        }

        gsl_vector_free(refined_coords);
        gsl_vector_free(atoms_coords_v);
        gsl_matrix_free(atoms_coords);
        gsl_matrix_free(distances);
        gsl_matrix_free(dist_limits);

        FILE * out = fopen("out.mol2", "w");
        char * buffer = NULL;
        size_t bsize = 0;

        mol2_molecule_serialize(mol->molecule, &buffer, &bsize);
        fprintf(out, "@<TRIPOS>MOLECULE\n%s\n@<TRIPOS>ATOM\n", buffer);

        for (size_t i = 0; i < mol->molecule->atoms_num; ++i) {
            mol2_atom_serialize(mol->atoms + i, &buffer, &bsize);
            fprintf(out, "%s\n", buffer);
        }

        fprintf(out, "@<TRIPOS>BOND\n");

        for (size_t i = 0; i < mol->molecule->bonds_num; ++i) {
            mol2_bond_serialize(mol->bonds + i, &buffer, &bsize);
            fprintf(out, "%s\n", buffer);
        }

        fclose(out);
    } while ((db = MOL2_DB_NEXT(db)));

    bi_close_db();

    return 0;
}
