#ifndef _LAB_DG2_UTIL_H_
#define _LAB_DG2_UTIL_H_

#include <stdio.h>
#include <stdlib.h>

#define SQR(x) ((x) * (x))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define DEG_2_RAD(a) (M_PI * (a) / 180.0)


#define LOG(s, fmt, ...) fprintf(s, "[" __FILE__ ":%d] " fmt "\n", __LINE__, ##__VA_ARGS__)
#define LOG_INF(fmt, ...) LOG(stdout, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) LOG(stderr, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) { LOG_ERR(fmt, ##__VA_ARGS__); exit(EXIT_FAILURE); }


#define LOG_VDW_RADII       1 << 0
#define LOG_BOND_LENGTHS    1 << 1
#define LOG_BONDS_ANGLES    1 << 2
#define LOG_MATRICES        1 << 3

//#define LOG_MASK ( \
    //LOG_VDW_RADII | \
    //LOG_BOND_LENGTHS | \
    //LOG_BONDS_ANGLES | \
    //LOG_MATRICES \
//)

#define LOG_MASK LOG_MATRICES


#define /* double */ UPPER_GET(/* gsl_matrix * */ m, /* size_t */ i, /* size_t */ j) \
    gsl_matrix_get((m), MIN(i, j), MAX(i, j))
#define /* double */ LOWER_GET(/* gsl_matrix * */ m, /* size_t */ i, /* size_t */ j) \
    gsl_matrix_get((m), MAX(i, j), MIN(i, j))

#define /* void */ UPPER_SET(/* gsl_matrix * */ m, /* size_t */ i, /* size_t */ j, /* double */ v) \
    gsl_matrix_set((m), MIN(i, j), MAX(i, j), (v))
#define /* void */ LOWER_SET(/* gsl_matrix * */ m, /* size_t */ i, /* size_t */ j, /* double */ v) \
    gsl_matrix_set((m), MAX(i, j), MIN(i, j), (v))


#define /* void */ PRINT_MATRIX(/* const char * */ s, /* const gsl_matrix * */ m) do { \
    fprintf(stdout, "[" __FILE__ ":%d] %s\n┌───┬─", __LINE__, s); \
    for (size_t j = 0; j < m->size2; ++j) { \
        fprintf(stdout, "────"); \
    } \
    fprintf(stdout, "┐\n|   | "); \
    for (size_t j = 0; j < m->size2; ++j) { \
        fprintf(stdout, "%5ld ", j); \
    } \
    fprintf(stdout, "|\n├───┼─"); \
    for (size_t j = 0; j < m->size2; ++j) { \
        fprintf(stdout, "──────"); \
    } \
    fprintf(stdout, "┤\n"); \
    for (size_t i = 0; i < m->size1; ++i) { \
        fprintf(stdout, "|%3ld│ ", i); \
        for (size_t j = 0; j < m->size2; ++j) { \
            fprintf(stdout, "%5.2lf ", gsl_matrix_get(m, i, j)); \
        } \
        fprintf(stdout, "|\n"); \
    } \
    fprintf(stdout, "└───┴─"); \
    for (size_t j = 0; j < m->size2; ++j) { \
        fprintf(stdout, "────"); \
    } \
    fprintf(stdout, "┘\n\n"); \
} while (0)

#define /* void */ PRINT_VECTOR(/* const char * */ s, /* const gsl_vector * */ v) do { \
    fprintf(stdout, "[" __FILE__ ":%d] %s\n", __LINE__, s); \
    for (size_t i = 0; i < v->size; ++i) { \
        fprintf(stdout, "%3.3lf ", gsl_vector_get(v, i)); \
    } \
    fprintf(stdout, "\n"); \
} while (0)

#define FOREACH(type, curr_item, arr, length) for ( \
    type * curr_item = arr; \
    curr_item != arr + length; \
    ++curr_item \
)

#endif // _LAB_DG2_UTIL_H_
