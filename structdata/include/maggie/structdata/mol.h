#ifndef _SDF_MOL_H_
#define _SDF_MOL_H_

#include <glib.h>

#include "ctab.h"
#include "parser.h"

typedef struct {
    gchar * name;
    gchar * signature_line; // TODO implement correct parse;
    gchar * comment;
} sd_mol_header_t;

typedef struct {
    sd_mol_header_t * header;
    sd_ctab_t * ctab;
} sd_mol_t;

sd_parser_t * sd_new_mol_parser(void);

#define SDF_MOL_PARSER_FEED_EOF(p, err) (sd_mol_t *) sd_parser_feed_eof(p, err)

#endif // _SDF_MOL_H_
