#include <structdata/parser.h>

sd_parser_t * sd_new_parser(
    int (* line_feed_cb)(gsize, GString *, gpointer, GError **),
    gpointer (* eof_feed_cb)(gpointer, gsize, GError **),
    void (* free_metadata_cb)(gpointer metadata),
    gpointer metadata
) {
    sd_parser_t * parser = g_new(sd_parser_t, 1);

    parser->_line_feed_cb = line_feed_cb;
    parser->_eof_feed_cb = eof_feed_cb;
    parser->_free_metadata_cb = free_metadata_cb;
    parser->_lines_fed = 0;
    parser->_metadata = metadata;

    return parser;
}

int sd_parser_feed_line(sd_parser_t * parser, GString * line, GError ** error) {
    return parser->_line_feed_cb(
        parser->_lines_fed++,
        line,
        parser->_metadata,
        error
    );
}

gpointer sd_parser_feed_eof(sd_parser_t * parser, GError ** error) {
    return parser->_lines_fed ?
        parser->_eof_feed_cb(
            parser->_metadata,
            parser->_lines_fed,
            error
        ) :
        NULL;
}

void sd_free_parser(sd_parser_t * parser) {
    if (parser->_free_metadata_cb && parser->_metadata) {
        parser->_free_metadata_cb(parser->_metadata);
    }
    g_free(parser);
}
