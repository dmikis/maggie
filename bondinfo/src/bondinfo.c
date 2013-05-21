#include <math.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#include <maggie/bondinfo.h>

#define LOG_ERROR(f, ...) fprintf( \
    stderr, \
    "%s:%d: " f "\n", \
    __FILE__, \
    __LINE__, \
    __VA_ARGS__ \
)

#define CHECK_DB(rv) if (!DB) { \
    LOG_ERROR("%s", "There is no available database"); \
    return rv; \
}

#define QUERY_SIZE 1024

#define NaN nan(NULL)

static sqlite3 * DB = NULL;

static double process_query(const char * query, size_t query_max_size);

void bi_open_db(const char * db_filename) {
    if (sqlite3_open_v2(db_filename, &DB, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK) {
        LOG_ERROR("%s", sqlite3_errmsg(DB));
        bi_close_db();
    }
}

#define BOND_ANGLES_TABLE_NAME  "angles"
#define BOND_LENGTHS_TABLE_NAME "lengths"
#define BOND_ANGLE_COL_NAME     "angle"
#define BOND_LENGTH_COL_NAME    "length"
#define BOND_F_ATOM_COL_NAME    "first_atom"
#define BOND_S_ATOM_COL_NAME    "second_atom"
#define BOND_T_ATOM_COL_NAME    "third_atom"
#define BOND_TYPE_COL_NAME      "bond_type"

double bi_bond_length(BI_ATOM_SIG f_atom, BI_ATOM_SIG s_atom, BI_BOND bond_type) {
    CHECK_DB(NaN);

    static char query[QUERY_SIZE];

    snprintf(
        query,
        QUERY_SIZE,
        "SELECT " BOND_LENGTH_COL_NAME " FROM " BOND_LENGTHS_TABLE_NAME
            " WHERE ("
                "(" BOND_F_ATOM_COL_NAME " = \"%s\" and " BOND_S_ATOM_COL_NAME " = \"%s\") or "
                "(" BOND_F_ATOM_COL_NAME " = \"%s\" and " BOND_S_ATOM_COL_NAME " = \"%s\")"
            ") and " BOND_TYPE_COL_NAME " = %d;",
        f_atom, s_atom,
        s_atom, f_atom,
        bond_type
    );

    return process_query(query, QUERY_SIZE);
}

double bi_bonds_angle(BI_ATOM_SIG f_atom, BI_ATOM_SIG s_atom, BI_ATOM_SIG t_atom) {
    CHECK_DB(NaN);

    static char query[QUERY_SIZE];
    static char query_fmt[] = "SELECT " BOND_ANGLE_COL_NAME " FROM " BOND_ANGLES_TABLE_NAME
        " WHERE ("
            "("
                BOND_F_ATOM_COL_NAME " = \"%s\" and "
                BOND_S_ATOM_COL_NAME " = \"%s\" and "
                BOND_T_ATOM_COL_NAME " = \"%s\""
            ") or "
            "("
                BOND_F_ATOM_COL_NAME " = \"%s\" and "
                BOND_S_ATOM_COL_NAME " = \"%s\" and "
                BOND_T_ATOM_COL_NAME " = \"%s\""
            ")"
        ");";

    snprintf(
        query,
        QUERY_SIZE,
        query_fmt,
        f_atom, s_atom, t_atom,
        t_atom, s_atom, f_atom
    );

    double result = process_query(query, QUERY_SIZE);

    if (isnan(result)) {
        snprintf(
            query,
            QUERY_SIZE,
            query_fmt,
            "*", s_atom, t_atom,
            t_atom, s_atom, "*"
        );

        result = process_query(query, QUERY_SIZE);
    }

    if (isnan(result)) {
        snprintf(
            query,
            QUERY_SIZE,
            query_fmt,
            "*", s_atom, "*",
            "*", s_atom, "*"
        );

        result = process_query(query, QUERY_SIZE);
    }

    return result;
}

void bi_close_db(void) {
    sqlite3_close_v2(DB);
    DB = NULL;
}

double process_query(const char * query, size_t query_max_size) {
    sqlite3_stmt * query_handle = NULL;
    if (sqlite3_prepare_v2(DB, query, query_max_size, &query_handle, NULL) != SQLITE_OK) {
        LOG_ERROR("%s", sqlite3_errmsg(DB));
        return NaN;
    }

    double result = NaN;

    int err_code;
    while ((err_code = sqlite3_step(query_handle)) == SQLITE_ROW) {
        if (sqlite3_data_count(query_handle) == 1) {
            result = sqlite3_column_double(query_handle, 0);
        } else {
            LOG_ERROR("Got %d result columns, 1 expected", sqlite3_data_count(query_handle));
            return NaN;
        }
    }

    sqlite3_finalize(query_handle);

    if (err_code != SQLITE_DONE) {
        LOG_ERROR("%s", sqlite3_errmsg(DB));
        return NaN;
    }

    return result;
}
