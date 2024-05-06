#include <stdbool.h>
#include <sys/types.h>
#include "Osms_File_list.h"
#include "Osms_File.h"

int count_id = 1;

// Función para verificar si existe el struct en la lista
bool file_exists(int process_id, char *file_name, FilesList *list, int list_len){

    for(int i = 0; i < list_len; i++){
        OsmsFile *file = fileslist_at_index(list, i);
        if(file->process_id == process_id & strcmp(file->file_name, file_name) == 0){
            return true;
        }
    }

    return false;
}

OsmsFile file_create(int process_id, char *file_name){

    OsmsFile file;
    file = (OsmsFile){.file_id = count_id, .process_id = process_id, .file_name = file_name, .current_position = 0};
    count_id = count_id + 1;
    return file;

}

int id_file(int process_id, char *file_name, FilesList *list, int list_len){

    for(int i = 0; i < list_len; i++){
        OsmsFile *file = fileslist_at_index(list, i);
        if(file->process_id == process_id & strcmp(file->file_name, file_name) == 0){
            return file->file_id;
        }
    }
}