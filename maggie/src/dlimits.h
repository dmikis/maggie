#ifndef _LAB_DG2_DLIMITS_H_
#define _LAB_DG2_DLIMITS_H_

#include <gsl/gsl_matrix.h>

#include <maggie/mol2.h>

#define LAB_DG2_DEFAULT_ULIMIT INFINITY
/* fill matrix with default upper limit: LAB_DG2_DEFAULT_ULIMIT */
void set_default_ulimits(gsl_matrix * dist_limits);

/* fill matrix with default lower limit: sum of van-der-waals radii */
void set_default_llimits(gsl_matrix * dist_limits, const mol2_t * mol);

/* fill matrix with bond length */
void set_bond_lengths(gsl_matrix * dist_limits, const mol2_t * mol);

void set_angle_limits(gsl_matrix * dist_limits, const mol2_t * mol);

void apply_triangle_inequality(gsl_matrix * limits);

#endif // _LAB_DG2_DLIMITS_H_
