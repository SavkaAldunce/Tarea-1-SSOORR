#pragma once
#include <sys/types.h>

typedef struct
{
    int file_descriptor;    // Descriptor de archivo
    off_t current_position; // Posición actual del puntero de archivo
} OsmsFile;