#ifndef _SDF_CTAB_H_
#define _SDF_CTAB_H_

#include <glib.h>

#include "parser.h"

/* Ctab */

typedef struct {
    /* number of atoms (current max 255) */
    guint8 atoms_num;
#   define SDF_ATOMS_NUM_MAX 255

    /* number of bonds (current max 255) */
    guint8 bonds_num;
#   define SDF_BONDS_NUM_MAX 255

    /* number of atoms lists (max 30) */
    guint8 atoms_lists_num;

    /* chiral flag */
    guint8 chiral_flag;
#   define SDF_CHIRAL 1
#   define SDF_NOT_CHIRAL 0

    /* number of stext entries */
    guint8 stext_entries_num;

    /* number of reaction components + 1 */
    guint8 reaction_components_num;

    /* number of reactants */
    guint8 reactants_num;

    /* number of products */
    guint8 products_num;

    /* number of intermediates */
    guint8 intermediates_num;

    /* number of lines of additional properties, including the M END line */
    guint8 additional_lines_num;

    /* current Ctab version: "V2000" */
    gchar version[16];
#   define SDF_CURRENT_CTAB_VER "V2000"
} sd_ctab_counts_t;

typedef struct {
    /* atom cartesian coordinates */
    gdouble x;
    gdouble y;
    gdouble z;

    /* atom symbol */
    gchar symbol[4];
#   define SDF_ATOM_LIST        "L"
#   define SDF_ATOM_UNSPECIFIED "*"
#   define SDF_ATOM_R_GROUP     "R#"

    gint8   mass_difference;

    gint8   charge;

    guint8  stereo_parity;
#   define SDF_NON_STEREO_PARITY                0
#   define SDF_EVEN_STEREO_PARITY               1
#   define SDF_ODD_STEREO_PARITY                2
#   define SDF_EITHER_OR_UNMARKED_STEREO_CENTER 3

    /* hydrogen count + 1 */
    guint8  hydrogen_count;

    /**/
    guint8  stereo_care_flag;
#   define SDF_IGNORE_STEREO_CONFIG     0
#   define SDF_STEREO_CONFIG_MUST_MATCH 1

    guint8  valence;

    guint8  h0_designator;

    guint8  react_component_type;
#   define SDF_REACTANT     1
#   define SDF_PRODUCT      2
#   define SDF_INTERMEDIATE 3

    guint8  react_component_number;

    guint8  mapping_number;

    guint8  invertion_flag;
#   define SDF_CONFIGURATION_INVERTED 1
#   define SDF_CONFIGURATION_RETAINED 2

    guint8  exact_change_flag;
} sd_ctab_atom_t;

typedef struct {
    /* first and second atom numbers */
    guint8 f_atom_number;
    guint8 s_atom_number;

    /* bond type */
    guint8 type;
#   define SDF_SINGLE_BOND   1
#   define SDF_DOUBLE_BOND   2
#   define SDF_TRIPLE_BOND   3
#   define SDF_AROMATIC_BOND 4

#   define SDF_SINGLE_OR_DOUBLE_BOND     5
#   define SDF_SINGLE_OR_AROMATIC_BOND   6
#   define SDF_DOUBLE_OR_AROMATIC_BOND   7

#   define SDF_ANY_BOND 8

    guint8 stereo;

    guint8 topology;
#   define SDF_RING_BOND  1
#   define SDF_CHAIN_BOND 2

    gint8 reacting_center_status;
} sd_ctab_bond_t;

typedef struct {
    sd_ctab_counts_t * counts;

    sd_ctab_atom_t * atoms;

    sd_ctab_bond_t * bonds;
} sd_ctab_t;

sd_parser_t * sd_new_ctab_parser(void);

#define SD_CTAB_PARSER_FEED_EOF(p, err) (sd_ctab_t *) sd_parser_feed_eof(p, err);

/**
 * Ctab parser errors domain.
 */
#define SD_CTAB_PARSER_ERROR sd_ctab_parser_error_quark()

/**
 * Ctab parser error codes.
 */
typedef enum {
    SD_CTAB_COUNTS_MALFORMED /* incorrect counts line */
} sd_ctab_parser_err_t;

#endif // _SDF_CTAB_H_
