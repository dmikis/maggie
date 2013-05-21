#include <structdata/mol.h>

typedef struct {
    sd_parser_t * header_parser;
    sd_parser_t * ctab_parser;
} metadata_t;

static metadata_t * new_metadata(void);

static int line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error);
static gpointer eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error);
static void free_metadata(gpointer md_pointer);

static int header_line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error);
static gpointer header_eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error);

sd_parser_t * sd_new_mol_parser(void) {
    return sd_new_parser(line_feed, eof_feed,
        free_metadata, (gpointer) new_metadata());
}

metadata_t * new_metadata(void) {
    metadata_t * metadata = g_new(metadata_t, 1);

    metadata->header_parser = sd_new_parser(
        header_line_feed,
        header_eof_feed,
        NULL,
        g_new(sd_mol_header_t, 1)
    );

    metadata->ctab_parser = sd_new_ctab_parser();

    return metadata;
}

int line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error) {
    metadata_t * metadata = (metadata_t *) md_pointer;

    static GError * local_err = NULL;

    sd_parser_feed_line(
        line_num < 3 ?
            metadata->header_parser :
            metadata->ctab_parser,
        line, &local_err
    );

    if (local_err) {
        g_propagate_prefixed_error(error, local_err, "[mol line %ld]", line_num);
        return 1;
    } else {
        return 0;
    }
}

gpointer eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error) {
    SD_UNUSED(lines_fed);

    metadata_t * metadata = (metadata_t *) md_pointer;
    sd_mol_t * mol = g_new(sd_mol_t, 1);

    mol->header = (sd_mol_header_t *) sd_parser_feed_eof(metadata->header_parser, error);
    mol->ctab = SD_CTAB_PARSER_FEED_EOF(metadata->ctab_parser, error);

    return (gpointer) mol;
}

void free_metadata(gpointer md_pointer) {
    metadata_t * metadata = (metadata_t *) md_pointer;

    sd_free_parser(metadata->header_parser);
    sd_free_parser(metadata->ctab_parser);

    g_free(metadata);
}

int header_line_feed(gsize line_num, GString * line, gpointer md_pointer, GError ** error) {
    SD_UNUSED(error);

    sd_mol_header_t * header = (sd_mol_header_t *) md_pointer;
    gchar * c_str_line = (gchar *) g_memdup(line->str, line->len + 1);

    switch (line_num) {
        case 0:
            header->name = c_str_line;
            return 0;
        case 1:
            header->signature_line = c_str_line;
            return 0;
        case 2:
            header->comment = c_str_line;
            return 0;
        default:
            // TODO raise error here
            return 1;
    };
}

#define HEADER_LINES_NUM 3
gpointer header_eof_feed(gpointer md_pointer, gsize lines_fed, GError ** error) {
    SD_UNUSED(error);

    if (lines_fed == HEADER_LINES_NUM) {
        return md_pointer;
    } else {
        // TODO raise error here
        g_free(md_pointer);
        return NULL;
    }
}
