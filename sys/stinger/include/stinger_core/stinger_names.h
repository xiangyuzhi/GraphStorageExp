#ifndef STINGER_NAMES_H_
#define STINGER_NAMES_H_

#ifdef __cplusplus
#define restrict
extern "C" {
#endif

#define NAME_STR_MAX 255

#ifdef NAME_USE_SQLITE
	#include "sqlite/sqlite3.h"

	#define NAME_EMPTY_TYPE (UINT64_MAX - 1)
	#define STINGER_NAME_DELETION

	typedef struct stinger_names {
	  int64_t next_type_idx;
	  int64_t free_type_idx;
	  sqlite3 * db;
	  int64_t max_types;
	  int64_t num_types;

	  int64_t next_type_start;
	  uint8_t storage[0];
	} stinger_names_t;

#else
	#undef STINGER_NAME_DELETION

	typedef struct stinger_names {
	  int64_t next_string;
	  int64_t next_type;
	  int64_t max_types;
	  int64_t max_names;

	  int64_t to_name_start;
	  int64_t from_name_start;
	  int64_t to_int_start;
	  uint8_t storage[0];
	} stinger_names_t;

#endif

stinger_names_t *
stinger_names_new(int64_t max_types);

void
stinger_names_init(stinger_names_t * sn, int64_t max_types);

void
stinger_names_resize(stinger_names_t ** sn, int64_t max_types);

size_t
stinger_names_size(int64_t max_types);

stinger_names_t *
stinger_names_free(stinger_names_t ** sn);

int
stinger_names_create_type(stinger_names_t * sn, const char * name, int64_t * out);

int64_t
stinger_names_lookup_type(stinger_names_t * sn, const char * name);

char *
stinger_names_lookup_name(stinger_names_t * sn, int64_t type);

int64_t
stinger_names_count(stinger_names_t * sn);

int64_t
stinger_names_remove_type(stinger_names_t * sn, int64_t type);

int64_t
stinger_names_remove_name(stinger_names_t * sn, const char * name);

void
stinger_names_save(stinger_names_t * sn, FILE * fp);

void
stinger_names_load(stinger_names_t * sn, FILE * fp);

void
stinger_names_print(stinger_names_t * sn);

#ifdef __cplusplus
}
#undef restrict
#endif

#endif
