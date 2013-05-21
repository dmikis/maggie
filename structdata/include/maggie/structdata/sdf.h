#ifndef _SD_SDF_H_
#define _SD_SDF_H_

#include <glib.h>

#include "mol.h"
#include "parser.h"

#define SD_SDF_DELIMITER "$$$$"

typedef struct {
    /* NULL-terminated array of MOL-record pointers */
    sd_mol_t ** molecules;
} sd_sdf_t;

sd_parser_t * sd_new_sdf_parser(void);

#define SD_SDF_PARSER_FEED_EOF(p, err) (sd_sdf_t *) sd_parser_feed_eof(p, err)

#endif // _SD_SDF_H_
