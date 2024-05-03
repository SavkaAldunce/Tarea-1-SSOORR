#pragma once
#include "Osms_File.h"

typedef struct fileslist {
  OsmsFile value;
  struct fileslist* next;
} FilesList;

FilesList* fileslist_init(OsmsFile value);
void fileslist_append(FilesList* list ,OsmsFile value );
OsmsFile* fileslist_at_index(FilesList* list, int index);
void fileslist_destroy(FilesList* list);