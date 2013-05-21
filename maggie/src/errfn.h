#ifndef _LAB_DG2_ERRFN_H_
#define _LAB_DG2_ERRFN_H_

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

gsl_vector * refine_coords(gsl_vector * coords, gsl_matrix * dist_limits);

#endif // _LAB_DG2_ERRFN_H_
