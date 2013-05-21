#ifndef _PARSER_H_
#define _PARSER_H_

/**
 * @file parser.h
 * @author Kirill Dmitrenko <kdmitrenko@gmail.com>
 * @date May, 2013
 * @brief Simple per-line parser API.
 */

#include <stdlib.h>

/**
 * @typedef metadata_ptr_t
 */
typedef void * metadata_ptr_t;

/**
 * @typedef line_feed_cb_t
 */
typedef int (* line_feed_cb_t)(size_t, const char *, metadata_ptr_t);

/**
 * @typedef eof_feed_cb_t
 */
typedef void * (* eof_feed_cb_t)(size_t, metadata_ptr_t);

/**
 * @typedef free_metadata_cb_t
 */
typedef void (* free_metadata_cb_t)(metadata_ptr_t);

typedef struct {
    line_feed_cb_t _line_feed_cb;
    eof_feed_cb_t _eof_feed_cb;
    free_metadata_cb_t _free_metadata_cb;
    size_t _lines_fed;
    metadata_ptr_t _metadata;
} parser_t;

/**
 * Create new parser.
 *
 * @param [in] line_feed_cb Callback to be called every time parser gets line.
 * @param [in] eof_feed_cb Callback to be called on parsing finish.
 * @param [in] free_metadata_cb Callback to be called on parser freeing to free
 *      metadata.
 * @param [in] metadata Pointer to parser's metadata.
 */
parser_t * parser_alloc(line_feed_cb_t line_feed_cb, eof_feed_cb_t eof_feed_cb,
    free_metadata_cb_t free_metadata_cb, metadata_ptr_t metadata);

/**
 * Feed new line to parser.
 *
 * @param [in] parser
 * @param [in] line Line to be fed.
 */
int parser_feed_line(parser_t * parser, const char * line);

/**
 * Finish parsing on parser.
 *
 * @param [in] parser Pointer to parser.
 * @returns Pointer to parsing result or NULL in case of errors.
 */
void * parser_feed_eof(parser_t * parser);

/**
 * Free previously allocated parser.
 *
 * @param [in] parser Parser to be freed.
 */
void parser_free(parser_t * parser);

#endif // _PARSER_H_
