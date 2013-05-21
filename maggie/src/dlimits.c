#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <maggie/bondinfo.h>

#include "dlimits.h"
#include "util.h"

void set_default_ulimits(gsl_matrix * dist_limits) {
    for (size_t i = 0; i < dist_limits->size1; ++i) {
        for (size_t j = i + 1; j < dist_limits->size2; ++j) {
            gsl_matrix_set(dist_limits, i, j, LAB_DG2_DEFAULT_ULIMIT);
        }
    }
}

static double vdw_radius(const char * s) {
    static char *symbols[] = { "H", "C", "N", "O" , NULL };
    static double van_der_waals_radii[] = { 1.09, 1.70, 1.55, 1.52 , 0.0};

    size_t i = 0;

    while (symbols[i] && strcmp(s, symbols[i])) {
        i++;
    }

    return van_der_waals_radii[i];
}

void set_default_llimits(gsl_matrix * dist_limits, const mol2_t * mol) {
    for (size_t i = 1; i < dist_limits->size1; ++i) {
        for (size_t j = 0; j < i; ++j) {
                char * f_atom_s = mol->atoms[i].name;
                char * s_atom_s = mol->atoms[j].name;

                double f_atom_r = vdw_radius(f_atom_s);
                double s_atom_r = vdw_radius(s_atom_s);

                gsl_matrix_set(dist_limits, i, j, f_atom_r + s_atom_r);

#               if defined(DEBUG) && (LOG_MASK & LOG_VDW_RADII)
                    LOG_INF("Van-der-Waals radii: %s => %lf, %s => %lf",
                        f_atom_s, f_atom_r,
                        s_atom_s, s_atom_r
                    );
#               endif
        }
    }
}

void set_bond_lengths(gsl_matrix * dist_limits, const mol2_t * mol) {
    FOREACH(mol2_bond_t, bond, mol->bonds, mol->molecule->bonds_num) {
        size_t origin_atom_index = mol2_atom_index_by_id(mol, bond->origin_atom_id);
        size_t target_atom_index = mol2_atom_index_by_id(mol, bond->target_atom_id);

        double bond_length = bi_bond_length(
            mol->atoms[origin_atom_index].sybyl_type,
            mol->atoms[target_atom_index].sybyl_type,
            bond->type
        );

#       if defined(DEBUG) && (LOG_MASK & LOG_BOND_LENGTHS)
            LOG_INF("Bond length: (%s, %s, %d) => %lf",
                mol->atoms[origin_atom_index].sybyl_type,
                mol->atoms[target_atom_index].sybyl_type,
                bond->type,
                bond_length
            );
#       endif

        if (isinf(bond_length)) {
            fprintf(stderr, "Bad bond lenght for %ld %ld\n", origin_atom_index, origin_atom_index);
        }

        UPPER_SET(dist_limits, origin_atom_index, target_atom_index, 1.01 * bond_length);
        LOWER_SET(dist_limits, origin_atom_index, target_atom_index, 0.99 * bond_length);
    }
}

void set_angle_limits(gsl_matrix * dist_limits, const mol2_t * mol) {
    size_t N = mol->molecule->atoms_num;
    gsl_matrix_char * bonds_m = gsl_matrix_char_calloc(N, N);

    FOREACH(mol2_bond_t, bond, mol->bonds, mol->molecule->bonds_num) {
        size_t origin_atom_index = mol2_atom_index_by_id(mol, bond->origin_atom_id);
        size_t target_atom_index = mol2_atom_index_by_id(mol, bond->target_atom_id);

        gsl_matrix_char_set(bonds_m, origin_atom_index, target_atom_index, 1);
        gsl_matrix_char_set(bonds_m, target_atom_index, origin_atom_index, 1);
    }

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (gsl_matrix_char_get(bonds_m, i, j)) {
                for (size_t k = 0; k < N; ++k) {
                    if (gsl_matrix_char_get(bonds_m, j, k) && i != k) {
                        double angle = bi_bonds_angle(
                            mol->atoms[i].sybyl_type,
                            mol->atoms[j].sybyl_type,
                            mol->atoms[k].sybyl_type
                        );

#                       if defined(DEBUG) && (LOG_MASK & LOG_BONDS_ANGLES)
                            LOG_INF("Bonds angled: (%s, %s, %s) => %lf",
                                mol->atoms[i].sybyl_type,
                                mol->atoms[j].sybyl_type,
                                mol->atoms[k].sybyl_type,
                                angle
                            );
#                       endif

                        double ulen1 = UPPER_GET(dist_limits, i, j);
                        double ulen2 = UPPER_GET(dist_limits, j, k);
                        double llen1 = LOWER_GET(dist_limits, i, j);
                        double llen2 = LOWER_GET(dist_limits, j, k);

#                       define ANGLE_E 2.0

                        double upper = sqrt(SQR(ulen1) + SQR(ulen2) -
                            2 * ulen1 * ulen2 * cos(DEG_2_RAD(angle + ANGLE_E)));
                        double lower = sqrt(SQR(llen1) + SQR(llen2) -
                            2 * llen1 * llen2 * cos(DEG_2_RAD(angle - ANGLE_E)));

                        UPPER_SET(dist_limits, i, k, upper);
                        LOWER_SET(dist_limits, i, k, lower);
                    }
                }
            }
        }
    }

    gsl_matrix_char_free(bonds_m);
}

void apply_triangle_inequality(gsl_matrix * limits) {
    size_t N = limits->size1;
    for (size_t k = 0; k < N; ++k) {
        for (size_t i = 0;  i < N - 1; ++i) {
            for (size_t j = i + 1; j < N; ++j) {
                double ulimit = UPPER_GET(limits, i, k) + UPPER_GET(limits, k, j);
                if (UPPER_GET(limits, i, j) > ulimit) {
                    UPPER_SET(limits, i, j, ulimit);
                }

                double llimit1 = LOWER_GET(limits, i, k) - LOWER_GET(limits, k, j);
                double llimit2 = LOWER_GET(limits, j, k) - LOWER_GET(limits, k, i);
                if (LOWER_GET(limits, i, j) < llimit1) {
                    LOWER_SET(limits, i, j, llimit1);
                } else if (LOWER_GET(limits, i, j) < llimit2) {
                    LOWER_SET(limits, i, j, llimit2);
                }

                if (LOWER_GET(limits, i, j) > UPPER_GET(limits, i, j)) LOG_FATAL("Bad bounds for %ld %ld", i, j);
            }
        }
    }
}
