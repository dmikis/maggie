#ifndef _MAGGIE_UTILS_STRBUF_H_
#define _MAGGIE_UTILS_STRBUF_H_

#include <stdlib.h>

typedef struct {
    void * _head, * _tail;
    size_t length;
} mgg_strbuf_t;

/**
 * Allocate new string buffer.
 *
 * @returns Pointer to new string buffer.
 */
mgg_strbuf_t * mgg_strbuf_alloc(void);

/**
 * Append single char to buffer.
 *
 * @param [in] buf Buffer.
 * @param [in] c Char to be appended.
 */
void mgg_strbuf_cappend(mgg_strbuf_t * buf, char c);

/**
 * Append C string to buffer.
 *
 * @param [in] buf Buffer.
 * @param [in] str String to be appened. Must be 0-terminated.
 */
void mgg_strbuf_sappend(mgg_strbuf_t * buf, const char * str);

/**
 * Append C string to buffer.
 *
 * @param [in] buf Buffer.
 * @param [in] str String to be appended.
 * @param [in] length String length.
 */
void mgg_strbuf_nappend(mgg_strbuf_t * buf, const char * str, size_t length);

/**
 * Get buffer data.
 *
 * @param [in] Pointer to buffer.
 * @returns Buffer data as C string.
 */
char * mgg_strbuf_cstr(const mgg_strbuf_t * buf);

/**
 * Wipe out all buffer data.
 *
 * @param [in] buf Buffer.
 */
void mgg_strbuf_wipe(mgg_strbuf_t * buf);

/**
 * Free allocated string buffer.
 *
 * @param [in] buf Pointer to buffer to be freed.
 */
void mgg_strbuf_free(mgg_strbuf_t * buf);

#endif // _MAGGIE_UTILS_STRINGBUFF_H_
