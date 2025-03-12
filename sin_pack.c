#include <stdio.h>
#include <string.h>
#include "bin.h"
#include "sin_pack.h"

#define PACK_IDENT "PACK"
#define SIN_IDENT "SIN"
#define SIN_PACK_HEADER_IDENT_SIZE 4
#define SIN_PACK_HEADER_OFFSET_SIZE 4
#define SIN_PACK_HEADER_SIZE_SIZE 4
#define SIN_PACK_HEADER_SIZE (SIN_PACK_HEADER_IDENT_SIZE + SIN_PACK_HEADER_OFFSET_SIZE + SIN_PACK_HEADER_SIZE_SIZE)

#define PACK_ENTRY_NAME_SIZE 56
#define SIN_ENTRY_NAME_SIZE 120
#define SIN_PACK_ENTRY_OFFSET_SIZE 4
#define SIN_PACK_ENTRY_SIZE_SIZE 4
#define PACK_ENTRY_SIZE (PACK_ENTRY_NAME_SIZE + SIN_PACK_ENTRY_OFFSET_SIZE + SIN_PACK_ENTRY_SIZE_SIZE)
#define SIN_ENTRY_SIZE (SIN_ENTRY_NAME_SIZE + SIN_PACK_ENTRY_OFFSET_SIZE + SIN_PACK_ENTRY_SIZE_SIZE)

bool_t is_sin_pack(restable_t * rt, const char *sin_pack_ident, const size_t entry_size)
{
  char ident[SIN_PACK_HEADER_IDENT_SIZE];
  size_t size;

  if (readf(rt->file, "c4c4l4", ident, NULL, &size) != OK)
    return FALSE;
  if (strncmp(ident, sin_pack_ident, SIN_PACK_HEADER_IDENT_SIZE) != 0)
    return FALSE;
  if (size % entry_size != 0)
    return FALSE;
  return TRUE;
}

bool_t is_pack(restable_t * rt)
{
  return is_sin_pack(rt, PACK_IDENT, PACK_ENTRY_SIZE);
}

bool_t is_sin(restable_t * rt)
{
  return is_sin_pack(rt, SIN_IDENT, SIN_ENTRY_SIZE);
}

bool_t sin_pack_read_dir(restable_t * rt, const char *sin_pack_ident, const size_t entry_size, const char *entry_format)
{
  char ident[SIN_PACK_HEADER_IDENT_SIZE];
  size_t offset;
  size_t size;
  size_t number;
  size_t i;

  if (readf(rt->file, "c4l4l4", ident, &offset, &size) != OK)
  {
    fprintf(stderr, "sin_pack_read_dir: Can't read header.\n");
    return FALSE;
  }
  if (strncmp(ident, sin_pack_ident, SIN_PACK_HEADER_IDENT_SIZE) != 0)
  {
    fprintf(stderr, "sin_pack_read_dir: Wrong ident.\n");
    return FALSE;
  }
  if (size % entry_size != 0)
  {
    fprintf(stderr, "sin_pack_read_dir: Wrong size.\n");
    return FALSE;
  }
  number = size / entry_size;
  if (rt_set_number(rt, number) == FALSE)
  {
    fprintf(stderr, "sin_pack_read_dir: Can't resize entries.\n");
    return FALSE;
  }
  fseek(rt->file, offset, SEEK_SET);
  for(i = 0; i < rt->number; i++)
  {
    if (readf(rt->file, entry_format,
              &(rt->entries[i].name),
              &(rt->entries[i].offset), &(rt->entries[i].size)) != OK)
    {
      fprintf(stderr, "sin_pack_read_dir: Can't read entry #%zu.\n", i);
      return FALSE;
    }
    rt->entries[i].compressed = rt->entries[i].size;
  }
  return TRUE;
}

bool_t pack_read_dir(restable_t * rt)
{
  return sin_pack_read_dir(rt, PACK_IDENT, PACK_ENTRY_SIZE, "s56l4l4");
}

bool_t sin_read_dir(restable_t * rt)
{
  return sin_pack_read_dir(rt, SIN_IDENT, SIN_ENTRY_SIZE, "s120l4l4");
}

bool_t sin_pack_fill_filename(resentry_t * re)
{
  s_strcpy(&(re->filename), re->name);
  if (re->filename == NULL)
    return FALSE;
  return TRUE;
}

bool_t pack_fill_filename(resentry_t * re)
{
  return sin_pack_fill_filename(re);
}

bool_t sin_fill_filename(resentry_t * re)
{
  return sin_pack_fill_filename(re);
}

bool_t sin_pack_fill_name(resentry_t * re, const size_t entry_name_size)
{
  s_strcpy(&(re->name), re->filename);
  if (strlen(re->name) > entry_name_size)
  {
    fprintf(stderr, "sin_pack_fill_name: Too long name \"%s\".\n", re->name);
    return FALSE;
  }
  if (re->name == NULL)
    return FALSE;
  return TRUE;
}

bool_t pack_fill_name(resentry_t * re)
{
  return sin_pack_fill_name(re, PACK_ENTRY_NAME_SIZE);
}

bool_t sin_fill_name(resentry_t * re)
{
  return sin_pack_fill_name(re, SIN_ENTRY_NAME_SIZE);
}

bool_t sin_pack_prepare_dir(restable_t * rt)
{
  fseek(rt->file, SIN_PACK_HEADER_SIZE, SEEK_SET);
  return TRUE;
}

bool_t pack_prepare_dir(restable_t * rt)
{
  return sin_pack_prepare_dir(rt);
}

bool_t sin_prepare_dir(restable_t * rt)
{
  return sin_pack_prepare_dir(rt);
}

bool_t sin_pack_write_dir(restable_t * rt, const char *sin_pack_ident, const size_t entry_size, const char *entry_format)
{
  size_t i;
  size_t offset;

  offset = ftell(rt->file);
  fseek(rt->file, 0, SEEK_SET);
  if (writef
      (rt->file, "c4l4l4", sin_pack_ident, offset,
       rt->number * entry_size) != OK)
  {
    fprintf(stderr, "sin_pack_write_dir: Can't write header.\n");
    return FALSE;
  }
  fseek(rt->file, offset, SEEK_SET);
  for(i = 0; i < rt->number; i++)
  {
    if (writef(rt->file, entry_format,
               rt->entries[i].name,
               rt->entries[i].offset, rt->entries[i].size) != OK)
    {
      fprintf(stderr, "sin_pack_write_dir: Can't write entry #%zu.\n", i);
      return FALSE;
    }
  }
  return TRUE;
}

bool_t pack_write_dir(restable_t * rt)
{
  return sin_pack_write_dir(rt, PACK_IDENT, PACK_ENTRY_SIZE, "s56l4l4");
}

bool_t sin_write_dir(restable_t * rt)
{
  return sin_pack_write_dir(rt, SIN_IDENT, SIN_ENTRY_SIZE, "s120l4l4");
}
