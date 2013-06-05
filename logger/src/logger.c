#include <stdarg.h>
#include <stdio.h>

#include <maggie/logger.h>

#define MGG_LOGGER_OUTPUT stderr

static uint8_t log_level = 0;

uint8_t mgg_logger_level(void) {
    return log_level;
}

void mgg_logger_set_level(uint8_t level) {
    log_level = level;
}

#define _MGG_LOG_FN_IMPL(lvl, lvl_mask) { \
    if (log_level & lvl_mask) { \
        va_list arguments; \
        va_start(arguments, fmt); \
        fprintf(MGG_LOGGER_OUTPUT, "[%s:%4.d] (" #lvl ") ", filename, line); \
        vfprintf(MGG_LOGGER_OUTPUT, fmt, arguments); \
        fprintf(MGG_LOGGER_OUTPUT, "\n"); \
    } \
}


_MGG_LOG_FN_DEF(debug) _MGG_LOG_FN_IMPL(debug, MGG_LOG_LVL_DEBUG)
_MGG_LOG_FN_DEF(info) _MGG_LOG_FN_IMPL(info, MGG_LOG_LVL_INFO)
_MGG_LOG_FN_DEF(warn) _MGG_LOG_FN_IMPL(warn, MGG_LOG_LVL_WARN)
_MGG_LOG_FN_DEF(error) _MGG_LOG_FN_IMPL(error, MGG_LOG_LVL_DEBUG)
