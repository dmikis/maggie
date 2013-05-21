#ifndef _SDF_PARSER_H_
#define _SDF_PARSER_H_

#include <glib.h>

typedef struct {
    int (* _line_feed_cb)(gsize, GString *, gpointer, GError **);
    gpointer (* _eof_feed_cb)(gpointer, gsize, GError **);
    void (* _free_metadata_cb)(gpointer);
    gsize _lines_fed;
    gpointer _metadata;
} sd_parser_t;

sd_parser_t * sd_new_parser(
    int (* line_feed_cb)(gsize, GString *, gpointer, GError **),
    gpointer (* eof_feed_cb)(gpointer, gsize, GError **),
    void (* free_metadata_cb)(gpointer metadata),
    gpointer metadata
);

#define SD_UNUSED(x) (void)(x)

int sd_parser_feed_line(sd_parser_t * parser, GString * line, GError ** error);

gpointer sd_parser_feed_eof(sd_parser_t * parser, GError ** error);

void sd_free_parser(sd_parser_t * parser);

#endif // _SDF_PARSER_H_
