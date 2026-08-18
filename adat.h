#ifndef __ADAT__
#define __ADAT__

#include <stdio.h>
#include "restable.h"

bool_t is_adat(restable_t * rt);

#define ADAT_SUBDIRS TRUE
#define ADAT_MERGE TRUE
#define ADAT_META FALSE
#define ADAT_TIME TRUE
#define ADAT_PAGE 1

bool_t adat_read_dir(restable_t * rt);
bool_t adat_fill_filename(resentry_t * re);
bool_t adat_extract_resource(restable_t * rt, size_t i);

#define adat_save_meta rt_not_save_meta
#define adat_load_meta rt_not_load_meta

bool_t adat_fill_name(resentry_t * re);
bool_t adat_prepare_dir(restable_t * rt);
bool_t adat_add_resource(restable_t * rt, size_t i);
bool_t adat_write_dir(restable_t * rt);

#endif
