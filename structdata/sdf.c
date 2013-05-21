#include <structdata/sdf.h>

typedef struct {
    sd_parser_t * current_mol_parser;
    GSList * mol_parsers;
    gsize mol_parsers_num;
    GString * delimiter;
} metadata_t;

static metadata_t * new_metadata(void);

static int line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error);
static gpointer eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error);
static void free_metadata(gpointer md_pointer);

sd_parser_t * sd_new_sdf_parser(void) {
    return sd_new_parser(line_feed, eof_feed,
        free_metadata, (gpointer) new_metadata());
}

metadata_t * new_metadata(void) {
    metadata_t * metadata = g_new(metadata_t, 1);

    metadata->current_mol_parser = sd_new_mol_parser();
    metadata->mol_parsers = g_slist_append(NULL,
        (gpointer) (metadata->current_mol_parser));
    metadata->mol_parsers_num = 1;
    metadata->delimiter = g_string_new(SD_SDF_DELIMITER);

    return metadata;
}

int line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error) {
    SD_UNUSED(line_num);

    metadata_t * metadata = (metadata_t *) md_pointer;

    /*static GError * local_err = NULL;*/

    if (!g_string_equal(line, metadata->delimiter)) {
        return sd_parser_feed_line(metadata->current_mol_parser, line, error);
    }

    metadata->mol_parsers = g_slist_append(metadata->mol_parsers,
        (gpointer) (metadata->current_mol_parser = sd_new_mol_parser()));
    metadata->mol_parsers_num++;

    return 0;
}

gpointer eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error) {
    SD_UNUSED(lines_fed);

    metadata_t * metadata = (metadata_t *) md_pointer;
    sd_sdf_t * sdf = g_new(sd_sdf_t, 1);

    sdf->molecules = g_new0(sd_mol_t *, metadata->mol_parsers_num + 1);

    GSList * mol_parser_item = metadata->mol_parsers;

    gsize curr_index = 0;
    do {
        sd_parser_t * mol_parser = (sd_parser_t *) mol_parser_item->data;
        sdf->molecules[curr_index++] = SDF_MOL_PARSER_FEED_EOF(mol_parser, error);
        /*sd_free_parser(mol_parser);*/
    } while ((mol_parser_item = g_slist_next(mol_parser_item)));

    return (gpointer) sdf;
}

void free_metadata(gpointer md_pointer) {
    metadata_t * metadata = (metadata_t *) md_pointer;

    g_slist_free(metadata->mol_parsers);
    g_string_free(metadata->delimiter, TRUE);
    g_free(metadata);
}
