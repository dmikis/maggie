#ifndef _LAB_DG_H_
#define _LAB_DG_H_

#include <gsl/gsl_matrix.h>

gsl_matrix * dists_to_coords(const gsl_matrix * dists);
gsl_matrix * coords_to_dists(const gsl_matrix * coords);

#endif // _LAB_DG_H_
