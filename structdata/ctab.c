#include <glib.h>
#include <stdio.h>

#include <structdata/ctab.h>

typedef struct {
    sd_ctab_counts_t * counts;
    sd_ctab_atom_t * atoms;
    sd_ctab_bond_t * bonds;
} metadata_t;

static metadata_t * new_metadata(void);

static int line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error);
static gpointer eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error);
static void free_metadata(gpointer md_pointer);

sd_parser_t * sd_new_ctab_parser(void) {
    return sd_new_parser(line_feed, eof_feed,
        free_metadata, (gpointer) new_metadata());
}

G_DEFINE_QUARK(CtabParsingError, sd_ctab_parser_error)

metadata_t * new_metadata(void) {
    metadata_t * metadata = g_new(metadata_t, 1);

    metadata->counts = g_new(sd_ctab_counts_t, 1);

    return metadata;
}

#define SDF_CTAB_COUNTS_RECORDS_NUM 12
int line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error) {
    SD_UNUSED(error);

    metadata_t * metadata = (metadata_t *) md_pointer;

    if (line_num == 0) {
        unsigned char tmp;
        sd_ctab_counts_t * counts = metadata->counts;

        gint records_read = sscanf(line->str,
            "%hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %s",
            &(counts->atoms_num),
            &(counts->bonds_num),
            &(counts->atoms_lists_num),
            &tmp, /* for obsolete record */
            &(counts->chiral_flag),
            &(counts->stext_entries_num),
            &(counts->reaction_components_num),
            &(counts->reactants_num),
            &(counts->products_num),
            &(counts->intermediates_num),
            &(counts->additional_lines_num),
            counts->version
        );

        if (SDF_CTAB_COUNTS_RECORDS_NUM == records_read) {
            metadata->atoms = g_new0(sd_ctab_atom_t, counts->atoms_num);
            metadata->bonds = g_new0(sd_ctab_bond_t, counts->bonds_num);
            return 0;
        } else {
            if (error) {
                *error = g_error_new(
                    SD_CTAB_PARSER_ERROR,
                    SD_CTAB_COUNTS_MALFORMED,
                    "[ctab line %lu] Read %d records, %d expected.",
                    line_num,
                    records_read,
                    SDF_CTAB_COUNTS_RECORDS_NUM
                );
            }
            return 1;
        }
    }

    if (line_num <= metadata->counts->atoms_num) {
        sd_ctab_atom_t * atom = metadata->atoms + line_num - 1;

        gint records_read = sscanf(line->str,
            "%lf %lf %lf %s %hhd %hhd %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
            &(atom->x),
            &(atom->y),
            &(atom->z),
            atom->symbol,
            &(atom->mass_difference),
            &(atom->charge),
            &(atom->stereo_parity),
            &(atom->hydrogen_count),
            &(atom->stereo_care_flag),
            &(atom->valence),
            &(atom->h0_designator),
            &(atom->react_component_type),
            &(atom->react_component_number),
            &(atom->mapping_number),
            &(atom->invertion_flag),
            &(atom->exact_change_flag)
        );

        SD_UNUSED(records_read);

        return 0;
    }

    if (line_num <= (metadata->counts->bonds_num + metadata->counts->atoms_num)) {
        sd_ctab_bond_t * bond = metadata->bonds + line_num - metadata->counts->atoms_num - 1;

        gint records_read = sscanf(line->str,
            "%hhu %hhu %hhu %hhu %hhu %hhd",
            &(bond->f_atom_number),
            &(bond->s_atom_number),
            &(bond->type),
            &(bond->stereo),
            &(bond->topology),
            &(bond->reacting_center_status)
        );

        SD_UNUSED(records_read);

        return 0;
    }

    return 0;
}

gpointer eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error) {
    SD_UNUSED(lines_fed);
    SD_UNUSED(error);

    metadata_t * metadata = (metadata_t *) md_pointer;
    sd_ctab_t * ctab = g_new(sd_ctab_t, 1);

    ctab->counts = metadata->counts;
    ctab->atoms = metadata->atoms;
    ctab->bonds = metadata->bonds;

    return (gpointer) ctab;
}

void free_metadata(gpointer md_pointer) {
    metadata_t * metadata = (metadata_t *) md_pointer;

    g_free(metadata);
}
