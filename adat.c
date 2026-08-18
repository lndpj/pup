#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bin.h"
#include "adat.h"
#include "comdec.h"

#define ADAT_IDENT "ADAT"
#define ADAT_VERSION 9
#define ADAT_HEADER_IDENT_SIZE 4
#define ADAT_HEADER_OFFSET_SIZE 4
#define ADAT_HEADER_SIZE_SIZE 4
#define ADAT_HEADER_VERSION_SIZE 4
#define ADAT_HEADER_SIZE (ADAT_HEADER_IDENT_SIZE + ADAT_HEADER_OFFSET_SIZE + ADAT_HEADER_SIZE_SIZE + ADAT_HEADER_VERSION_SIZE)

#define ADAT_ENTRY_NAME_SIZE 128
#define ADAT_ENTRY_OFFSET_SIZE 4
#define ADAT_ENTRY_SIZE_SIZE 4
#define ADAT_ENTRY_COMPRESSED_SIZE 4
#define ADAT_ENTRY_TIME_SIZE 4
#define ADAT_ENTRY_SIZE (ADAT_ENTRY_NAME_SIZE + ADAT_ENTRY_OFFSET_SIZE + ADAT_ENTRY_SIZE_SIZE + \
	ADAT_ENTRY_COMPRESSED_SIZE + ADAT_ENTRY_TIME_SIZE)

bool_t is_adat(restable_t * rt)
{
  char ident[ADAT_HEADER_IDENT_SIZE];
  size_t size;
  size_t version;

  if (readf(rt->file, "c4c4l4l4", ident, NULL, &size, &version) != OK)
    return FALSE;
  if (strncmp(ident, ADAT_IDENT, ADAT_HEADER_IDENT_SIZE) != 0)
    return FALSE;
  if (size % ADAT_ENTRY_SIZE != 0)
    return FALSE;
  if (version != ADAT_VERSION)
    return FALSE;
  return TRUE;
}

bool_t adat_read_dir(restable_t * rt)
{
  char ident[ADAT_HEADER_IDENT_SIZE];
  size_t offset;
  size_t size;
  size_t version;
  size_t number;
  size_t i;

  if (readf(rt->file, "c4l4l4l4", ident, &offset, &size, &version) != OK)
  {
    fprintf(stderr, "adat_read_dir: Can't read header.\n");
    return FALSE;
  }
  if (strncmp(ident, ADAT_IDENT, ADAT_HEADER_IDENT_SIZE) != 0)
  {
    fprintf(stderr, "adat_read_dir: Wrong ident.\n");
    return FALSE;
  }
  if (size % ADAT_ENTRY_SIZE != 0)
  {
    fprintf(stderr, "adat_read_dir: Wrong size.\n");
    return FALSE;
  }
  number = size / ADAT_ENTRY_SIZE;
  if (rt_set_number(rt, number) == FALSE)
  {
    fprintf(stderr, "adat_read_dir: Can't resize entries.\n");
    return FALSE;
  }
  fseek(rt->file, offset, SEEK_SET);
  for(i = 0; i < rt->number; i++)
  {
    if (readf(rt->file, "s128l4l4l4l4",
              &(rt->entries[i].name),
              &(rt->entries[i].offset),
              &(rt->entries[i].size),
              &(rt->entries[i].compressed),
              &(rt->entries[i].time)) != OK)
    {
      fprintf(stderr, "adat_read_dir: Can't read entry #%zu.\n", i);
      return FALSE;
    }
    if (rt->entries[i].compressed == 0)
      fprintf(stderr, "adat_read_dir: %zu resource %s with zero compressed size\n", i, rt->entries[i].name);
    else if (rt->entries[i].compressed == rt->entries[i].size)
      fprintf(stderr, "adat_read_dir: %zu resource %s with equal compressed size after compression\n", i, rt->entries[i].name);
    else if (rt->entries[i].compressed > rt->entries[i].size)
      fprintf(stderr, "adat_read_dir: %zu resource %s with greater compressed size after compression\n", i, rt->entries[i].name);
  }
  return TRUE;
}

bool_t adat_fill_filename(resentry_t * re)
{
  s_strcpy(&(re->filename), re->name);
  if (re->filename == NULL)
    return FALSE;
  char *s = re->filename;
  while (*s != '\0')
  {
    if (*s == '\\')
      *s = SYS_PATH_DELIM;
    s++;
  }
  return TRUE;
}

bool_t adat_extract_resource(restable_t * rt, size_t i)
{
  char *filename;

  filename = NULL;
  s_strcpy(&filename, rt->basepath);
  s_strcat(&filename, rt->entries[i].filename);
  fseek(rt->file, rt->entries[i].offset, SEEK_SET);
  if (c_fextract(filename, rt->file, rt->entries[i].size, rt->entries[i].compressed, &comdec_zlib) == FALSE)
  {
    fprintf(stderr, "adat_unpack_files: Can't extract entry #%zu.\n", i);
    s_free(&filename);
    return FALSE;
  }
  s_free(&filename);
  return TRUE;
}

bool_t adat_fill_name(resentry_t * re)
{
  s_strcpy(&(re->name), re->filename);
  if (strlen(re->name) > ADAT_ENTRY_NAME_SIZE)
  {
    fprintf(stderr, "adat_fill_name: Too long name \"%s\".\n", re->name);
    return FALSE;
  }
  char *s = re->name;
  while (*s != '\0')
  {
    if (*s == SYS_PATH_DELIM)
      *s = '\\';
    s++;
  }
  return TRUE;
}

bool_t adat_prepare_dir(restable_t * rt)
{
  fseek(rt->file, ADAT_HEADER_SIZE, SEEK_SET);
  return TRUE;
}

bool_t adat_add_resource(restable_t * rt, size_t i)
{
  char *filename;

  filename = NULL;
  rt->entries[i].offset = ftell(rt->file);
  s_strcpy(&filename, rt->basepath);
  s_strcat(&filename, rt->entries[i].filename);
  if (c_fadd(rt->file, filename, &(rt->entries[i].size),
             &(rt->entries[i].compressed), &comdec_zlib, 9) == FALSE)
  {
    fprintf(stderr, "adat_add_resource: Can't add entry #%zu.\n", i);
    s_free(&filename);
    return FALSE;
  }
  s_free(&filename);
  return TRUE;
}

bool_t adat_write_dir(restable_t * rt)
{
  size_t i;
  size_t offset;

  offset = ftell(rt->file);
  fseek(rt->file, 0, SEEK_SET);
  if (writef
      (rt->file, "c4l4l4l4", ADAT_IDENT, offset,
       rt->number * ADAT_ENTRY_SIZE, ADAT_VERSION) != OK)
  {
    fprintf(stderr, "adat_write_dir: Can't write header.\n");
    return FALSE;
  }
  fseek(rt->file, offset, SEEK_SET);
  for(i = 0; i < rt->number; i++)
  {
    if (writef(rt->file, "s128l4l4l4l4",
               rt->entries[i].name,
               rt->entries[i].offset,
               rt->entries[i].size,
               rt->entries[i].compressed,
               rt->entries[i].time) != OK)
    {
      fprintf(stderr, "adat_write_dir: Can't write entry #%zu.\n", i);
      return FALSE;
    }
  }
  return TRUE;
}
