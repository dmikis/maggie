#include <string.h>

#include <maggie/utils/strbuf.h>

#define DECL_N_ALLOC(t, v)      t * v = (t *) malloc(sizeof(t))
#define DECL_N_NALLOC(t, v, n)  t * v = (t *) malloc((n) * sizeof(t))

typedef struct {
    char * data;
    size_t pos, length;
    void * next;
} chunk_t;

#define BUF_HEAD(b) ((chunk_t *) (b)->_head)
#define BUF_TAIL(b) ((chunk_t *) (b)->_tail)

#define CHUNK_NEXT(c)    ((chunk_t *) (c)->next)
#define CHUNK_POS_PTR(c) ((c)->data + (c)->pos)
#define CHUNK_IS_FULL(c) ((c)->pos == (c)->length)

static size_t buffer_data_alloc_size = 64;

static chunk_t * chunk_alloc(size_t size);

static void chunk_free(chunk_t * chunk);

mgg_strbuf_t * mgg_strbuf_alloc(void) {
    DECL_N_ALLOC(mgg_strbuf_t, buf);

    buf->_head = buf->_tail = NULL;
    buf->length = 0;

    return buf;
}

void mgg_strbuf_cappend(mgg_strbuf_t * buf, char c) {
    chunk_t * chunk = BUF_TAIL(buf);

    if (CHUNK_IS_FULL(chunk)) {
        chunk->next = buf->_tail = (void *) chunk_alloc(0);
        chunk = CHUNK_NEXT(chunk);
    }

    chunk->data[chunk->pos++] = c;
    buf->length += 1;
}

void mgg_strbuf_sappend(mgg_strbuf_t * buf, const char * str) {
    size_t str_l = strlen(str);
    chunk_t * chunk = BUF_TAIL(buf);

    if (str_l > buffer_data_alloc_size || str_l > (chunk->length - chunk->pos)) {
        chunk->next = buf->_tail = (void *) chunk_alloc(str_l > buffer_data_alloc_size ? str_l : 0);
        chunk = CHUNK_NEXT(chunk);
    }

    strncpy(CHUNK_POS_PTR(chunk), str, str_l);
    chunk->pos += str_l;
    buf->length += str_l;
}

void mgg_strbuf_nappend(mgg_strbuf_t * buf, const char * str, size_t length) {
    size_t str_l = strnlen(str, length);
    chunk_t * chunk = BUF_TAIL(buf);

    if (str_l > buffer_data_alloc_size || str_l > (chunk->length - chunk->pos)) {
        chunk->next = buf->_tail = (void *) chunk_alloc(str_l > buffer_data_alloc_size ? str_l : 0);
        chunk = CHUNK_NEXT(chunk);
    }

    strncpy(CHUNK_POS_PTR(chunk), str, str_l);
    chunk->pos += str_l;
    buf->length += str_l;
}

char * mgg_strbuf_cstr(const mgg_strbuf_t * buf) {
    DECL_N_NALLOC(char, str, buf->length + 1);
    char * curr_str_pos = str;

    chunk_t * curr_chunk = BUF_HEAD(buf);

    while (curr_chunk) {
        curr_str_pos = stpncpy(curr_str_pos, curr_chunk->data, curr_chunk->pos);
        curr_chunk = CHUNK_NEXT(curr_chunk);
    }

    str[buf->length] = '\0';

    return str;
}

void mgg_strbuf_wipe(mgg_strbuf_t * buf) {
    chunk_t * chunk = BUF_HEAD(buf);

    while (chunk) {
        chunk_t * next = CHUNK_NEXT(chunk);
        chunk_free(chunk);
        chunk = next;
    }

    buf->_tail = buf->_head = NULL;
    buf->length = 0;
}

void mgg_strbuf_free(mgg_strbuf_t * buf) {
    mgg_strbuf_wipe(buf);
    free(buf);
}

static chunk_t * chunk_alloc(size_t length) {
    DECL_N_ALLOC(chunk_t, chunk);

    chunk->pos = 0;
    chunk->length = length ? length : buffer_data_alloc_size;
    chunk->data = (char *) malloc(chunk->length);
    chunk->next = NULL;

    return chunk;
}

static void chunk_free(chunk_t * chunk) {
    free(chunk->data);
    free(chunk);
}
