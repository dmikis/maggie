#ifndef _MAGGIE_LOGGER_H_
#define _MAGGIE_LOGGER_H_

#include <stdint.h>

/* Log levels */
#define MGG_LOG_LVL_DEBUG   1
#define MGG_LOG_LVL_INFO    2
#define MGG_LOG_LVL_WARN    4
#define MGG_LOG_LVL_ERROR   8

/**
 * Get current log level.
 *
 * @returns Log level bit mask.
 */
uint8_t mgg_logger_level(void);

/**
 * Set log level.
 *
 * @param [in] New level bit mask.
 */
void mgg_logger_set_level(uint8_t level);

/**
 * Set maximum log level: all messages will be logged.
 */
#define MGG_LOGGER_SET_ALL() mgg_logger_set_level( \
    MGG_LOG_LVL_DEBUG   | \
    MGG_LOG_LVL_INFO    | \
    MGG_LOG_LVL_WARN    | \
    MGG_LOG_LVL_ERROR   | \
)

/**
 * Set `verbose` log level: log infos, warnings and errors
 */
#define MGG_LOGGER_SET_VERBOSE() mgg_logger_set_level( \
    MGG_LOG_LVL_INFO    | \
    MGG_LOG_LVL_WARN    | \
    MGG_LOG_LVL_ERROR   | \
)

/**
 * Set quiet mode: error messages only will be logged.
 */
#define MGG_LOGGER_SET_QUIET() mgg_logger_set_level(MGG_LOG_LVL_ERROR)

#define _MGG_LOG_FN_DEF(lvl) void _mgg_log_ ## lvl( \
    const char * filename, \
    int line, \
    const char * fmt, \
    ... \
)

_MGG_LOG_FN_DEF(debug);
_MGG_LOG_FN_DEF(info);
_MGG_LOG_FN_DEF(warn);
_MGG_LOG_FN_DEF(error);

#define _MGG_LOG_TEMPLATE(lvl, fmt, ...) _mgg_log_ ## lvl( \
    __FILE__, \
    __LINE__, \
    fmt, \
    ##__VA_ARGS__ \
)

/**
 * Puts debug messsage.
 */
#define MGG_LOG_DEBUG(fmt, ...) _MGG_LOG_TEMPLATE(debug, fmt, ##__VA_ARGS__)

/**
 * Puts info message.
 */
#define MGG_LOG_INFO(fmt, ...) _MGG_LOG_TEMPLATE(info, fmt, ##__VA_ARGS__)

/**
 * Puts warn message.
 */
#define MGG_LOG_WARN(fmt, ...) _MGG_LOG_TEMPLATE(warn, fmt, ##__VA_ARGS__)

/**
 * Puts error message.
 */
#define MGG_LOG_ERROR(fmt, ...) _MGG_LOG_TEMPLATE(error, fmt, ##__VA_ARGS__)

#endif // _MAGGIE_LOGGER_H_
