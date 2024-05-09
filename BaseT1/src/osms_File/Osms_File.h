#pragma once
#include <sys/types.h>
#include <stdbool.h> // bool, true, false

// Declaración adelantada de FilesList
struct fileslist; // Usa struct fileslist; si 'typedef struct fileslist FilesList;' es usado en Osms_File_list.h

typedef struct
{
    int file_id;    // id del archivo
    int process_id;
    char *file_name;
    off_t current_position; // Posición actual del puntero de archivo
    off_t current_position_read;   
} OsmsFile;

bool file_exists(int process_id, char *file_name, struct fileslist *list, int list_len); // Usa struct fileslist
OsmsFile file_create(int process_id, char *file_name);
int id_file(int process_id, char *file_name, struct fileslist *list, int list_len);


