#include <gsl/gsl_multimin.h>

#include <maggie/logger.h>

#include "errfn.h"
#include "util.h"

static double error_fn(const gsl_vector * x, void * params);

static void error_dfn(const gsl_vector * x, void * params, gsl_vector * g);

static void error_fdfn(const gsl_vector * x, void * params, double * f, gsl_vector * g);

gsl_vector * refine_coords(gsl_vector * coords, gsl_matrix * dist_limits) {
    gsl_multimin_fdfminimizer * s = gsl_multimin_fdfminimizer_alloc(
        gsl_multimin_fdfminimizer_conjugate_fr,
        /*gsl_multimin_fdfminimizer_vector_bfgs,*/
        coords->size
    );

    gsl_multimin_function_fdf error_func;

    error_func.n = coords->size;
    error_func.f = error_fn;
    error_func.df = error_dfn;
    error_func.fdf = error_fdfn;
    error_func.params = (void *) dist_limits;

    gsl_multimin_fdfminimizer_set(s, &error_func, coords, 0.01, 1e-4);

    size_t iters_num = 0;
    int status;
    do {
        LOG_INF("item #%ld", iters_num);
        iters_num++;
        status = gsl_multimin_fdfminimizer_iterate(s);

        if (status) {
            MGG_LOG_INFO("Break! Status: %d; GSL_ENOPROG: %d", status, GSL_ENOPROG);
            break;
        }

        status = gsl_multimin_test_gradient(s->gradient, 1e-4);
    } while (status == GSL_CONTINUE && iters_num < 100000);

    if (iters_num == 100000) LOG_FATAL("Too many iterations");

    if (status == GSL_SUCCESS) {
        MGG_LOG_INFO("Minimum!");
    }

    PRINT_VECTOR("x", s->x);
    MGG_LOG_INFO("Error function value: %lf", s->f);

    gsl_vector * refined_coords = gsl_vector_alloc(s->x->size);
    gsl_vector_memcpy(refined_coords, s->x);

    gsl_multimin_fdfminimizer_free(s);

    return refined_coords;
}

double error_fn(const gsl_vector * x, void * params) {
    gsl_matrix * limits = (gsl_matrix *) params;
    double result = 0.0;

    for (size_t i = 1; i < limits->size1; ++i) {
        for (size_t j = 0; j < i; ++j) {
            double d =
                SQR(gsl_vector_get(x, 3 * i) - gsl_vector_get(x, 3 * j)) +
                SQR(gsl_vector_get(x, 3 * i + 1) - gsl_vector_get(x, 3 * j + 1)) +
                SQR(gsl_vector_get(x, 3 * i + 2) - gsl_vector_get(x, 3 * j + 2));
            double l = SQR(LOWER_GET(limits, i, j));
            double u = SQR(UPPER_GET(limits, i, j));

            double Eij = 0.0;

            if (d < l) {
                Eij = SQR(d - l);
            } else if (d > u) {
                Eij = SQR(d - u);
            }

            result += Eij;
        }
    }

    PRINT_VECTOR("error_fn args", x);
    LOG_INF("error_fn: %lf", result);

    return result;
}

void error_dfn(const gsl_vector * x, void * params, gsl_vector * g) {
    gsl_matrix * limits = (gsl_matrix *) params;

    for (size_t k = 0; k < 3; ++k) {
        for (size_t i = 0; i < limits->size1; ++i) {
            double grad_el = 0.0;
            for (size_t j = 0; j < i; ++j) {
                double d =
                    SQR(gsl_vector_get(x, 3 * i) - gsl_vector_get(x, 3 * j)) +
                    SQR(gsl_vector_get(x, 3 * i + 1) - gsl_vector_get(x, 3 * j + 1)) +
                    SQR(gsl_vector_get(x, 3 * i + 2) - gsl_vector_get(x, 3 * j + 2));
                double l = SQR(LOWER_GET(limits, i, j));
                double u = SQR(UPPER_GET(limits, i, j));

                if (d < l) {
                    grad_el -= 4.0 * (d - l) * (gsl_vector_get(x, 3 * j + k) - gsl_vector_get(x, 3 * i + k));
                } else if (d > u) {
                    grad_el -= 4.0 * (d - u) * (gsl_vector_get(x, 3 * j + k) - gsl_vector_get(x, 3 * i + k));
                }
            }
            for (size_t j = i+1; j < limits->size1; ++j) {
                double d =
                    SQR(gsl_vector_get(x, 3 * i) - gsl_vector_get(x, 3 * j)) +
                    SQR(gsl_vector_get(x, 3 * i + 1) - gsl_vector_get(x, 3 * j + 1)) +
                    SQR(gsl_vector_get(x, 3 * i + 2) - gsl_vector_get(x, 3 * j + 2));
                double l = SQR(LOWER_GET(limits, i, j));
                double u = SQR(UPPER_GET(limits, i, j));

                if (d < l) {
                    grad_el += 4.0 * (d - l) * (gsl_vector_get(x, 3 * i + k) - gsl_vector_get(x, 3 * j + k));
                } else if (d > u) {
                    grad_el += 4.0 * (d - u) * (gsl_vector_get(x, 3 * i + k) - gsl_vector_get(x, 3 * j + k));
                }
            }
            gsl_vector_set(g, 3 * i + k, grad_el);
        }
    }

    PRINT_VECTOR("error_dfn args", x);
    PRINT_VECTOR("error_dfn grad", g);
}

void error_fdfn(const gsl_vector * x, void * params, double * f, gsl_vector * g) {
    *f = error_fn(x, params);
    error_dfn(x, params, g);
}
