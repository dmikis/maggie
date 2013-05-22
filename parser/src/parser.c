#include <assert.h>
#include <stdlib.h>

#include <maggie/parser.h>

mgg_parser_t * mgg_parser_alloc(line_feed_cb_t line_feed_cb, eof_feed_cb_t eof_feed_cb,
    free_metadata_cb_t free_metadata_cb, metadata_ptr_t metadata) {

    mgg_parser_t * parser = (mgg_parser_t *) malloc(sizeof(mgg_parser_t));

    parser->_line_feed_cb = line_feed_cb;
    parser->_eof_feed_cb = eof_feed_cb;
    parser->_free_metadata_cb = free_metadata_cb;
    parser->_lines_fed = 0;
    parser->_metadata = metadata;

    return parser;
}

int mgg_parser_feed_line(mgg_parser_t * parser, const char * line) {
    return parser->_line_feed_cb(
        parser->_lines_fed++,
        line,
        parser->_metadata
    );
}

void * mgg_parser_feed_eof(mgg_parser_t * parser) {
    return parser->_lines_fed ?
        parser->_eof_feed_cb(
            parser->_lines_fed,
            parser->_metadata
        ) :
        NULL;
}

void mgg_parser_free(mgg_parser_t * parser) {
    assert(parser);
    if (parser->_free_metadata_cb && parser->_metadata) {
        parser->_free_metadata_cb(parser->_metadata);
    }
    free(parser);
}
