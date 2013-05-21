#ifndef _SDF_LOG_H_
#define _SDF_LOG_H_

#define LOG_ERROR(f, ...) \
    fprintf( \
        stderr, \
        "%s:%d: " f "\n", \
        __FILE__, \
        __LINE__, \
        __VA_ARGS__ \
    )

#define LOG_GLIB_ERROR(err) \
    fprintf( \
        stderr, \
        "%s:%d: %s [%d]\n", \
        __FILE__, \
        __LINE__, \
        err->message, \
        err->code \
    )

#endif // _SDF_LOG_H_
