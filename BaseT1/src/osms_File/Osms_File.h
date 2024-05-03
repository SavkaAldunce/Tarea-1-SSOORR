#pragma once
#include <sys/types.h>
#include <stdbool.h> // bool, true, false
#include "Osms_File_list.h"

typedef struct
{
    int file_id;    // id del archivo
    int process_id;
    char *file_name;
    off_t current_position; // Posición actual del puntero de archivo
    
} OsmsFile;

bool file_exists(int process_id, char *file_name, FilesList *list, int list_len);
OsmsFile file_create(int process_id, char *file_name, int list_len);
int id_file(int process_id, char *file_name, FilesList *list, int list_len);

