#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>

#include <maggie/dg.h>

#define /* void */ PRINT_MATRIX(/* const char * */ s, /* const gsl_matrix * */ m) {}

gsl_matrix * dists_to_coords(const gsl_matrix * dists) {
    gsl_matrix * I = gsl_matrix_alloc(dists->size1, dists->size2);
    gsl_matrix_set_identity(I);

    gsl_matrix * i = gsl_matrix_alloc(dists->size1, 1);
    gsl_matrix_set_all(i, 1.0);

    /* TODO support user defined masses */
    gsl_matrix * masses = gsl_matrix_alloc(dists->size1, 1);
    gsl_matrix_set_all(masses, 1.0);

    /* TODO correct mass computation */
    double M = 1.0 / dists->size1;

    gsl_matrix * A = gsl_matrix_alloc(dists->size1, dists->size2);
    gsl_matrix_set_identity(A);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, -M, masses, i, 1.0, A);

    PRINT_MATRIX("A:", A);

    gsl_matrix * tmp_matrix = gsl_matrix_alloc(dists->size1, dists->size2);
    gsl_matrix_set_zero(tmp_matrix);

    gsl_matrix * dists2 = gsl_matrix_alloc(dists->size1, dists->size2);
    gsl_matrix_memcpy(dists2, dists);
    gsl_matrix_mul_elements(dists2, dists);

    PRINT_MATRIX("sqr dists:", dists2);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dists2, A, 0.0, tmp_matrix);

    PRINT_MATRIX("sqr dists * A:", tmp_matrix);

    gsl_matrix_set_identity(A);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, -M, i, masses, 1.0, A);

    gsl_matrix * B = gsl_matrix_alloc(dists->size1, dists->size2);
    gsl_matrix_set_zero(B);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, -0.5, A, tmp_matrix, 0.0, B);

    gsl_matrix_free(tmp_matrix);
    gsl_matrix_free(A);
    gsl_matrix_free(masses);
    gsl_matrix_free(i);
    gsl_matrix_free(I);

    gsl_vector * eigen_vals = gsl_vector_alloc(dists->size1);
    gsl_matrix * eigen_vecs = gsl_matrix_alloc(dists->size1, dists->size1);

    PRINT_MATRIX("B:", B);

    gsl_eigen_symmv_workspace * workspace = gsl_eigen_symmv_alloc(dists->size1);

    gsl_eigen_symmv(B, eigen_vals, eigen_vecs, workspace);

    /*for(size_t i = 0; i < dists->size1; ++i) {*/
        /*fprintf(stdout, "%3.1lf ", gsl_vector_get(eigen_vals, i));*/
    /*}*/
    /*fprintf(stdout, "\n");*/
    PRINT_MATRIX("Eigen vectors:", eigen_vecs);

    gsl_eigen_symmv_free(workspace);

    gsl_matrix_free(B);

    gsl_matrix * result = gsl_matrix_alloc(dists->size1, 3);

    gsl_vector * tmp_vector = gsl_vector_alloc(dists->size1);
    for (size_t i = 0; i < 3; ++i) {
        size_t max_eigen_val_index = gsl_vector_max_index(eigen_vals);
        double eigen_val = gsl_vector_get(eigen_vals, max_eigen_val_index);
        gsl_vector_set(eigen_vals, max_eigen_val_index, -1.0);
        gsl_matrix_get_col(tmp_vector, eigen_vecs, max_eigen_val_index);
        gsl_vector_scale(tmp_vector, sqrt(eigen_val));
        gsl_matrix_set_col(result, i, tmp_vector);
    }
    gsl_vector_free(tmp_vector);

    gsl_vector_free(eigen_vals);
    gsl_matrix_free(eigen_vecs);

    return result;
}

gsl_matrix * coords_to_dists(const gsl_matrix * coords) {
    size_t points_num = coords->size1;
    gsl_matrix * dists = gsl_matrix_calloc(points_num, points_num);

    gsl_vector * first_point = gsl_vector_alloc(3);
    gsl_vector * second_point = gsl_vector_alloc(3);

    for (size_t i = 1; i < points_num; ++i) {
        for (size_t j = 0; j < i; ++j) {
            gsl_matrix_get_row(first_point, coords, i);
            gsl_matrix_get_row(second_point, coords, j);

            gsl_vector_sub(first_point, second_point);

            double distance = gsl_blas_dnrm2(first_point);

            gsl_matrix_set(dists, i, j, distance);
            gsl_matrix_set(dists, j, i, distance);
        }
    }

    return dists;
}
