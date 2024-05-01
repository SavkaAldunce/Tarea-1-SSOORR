#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> //para strcmp
#include <stdbool.h> // bool, true, false
#include <stdint.h>

char* path;

// funciones generales

void os_mount(char* memory_path){
    path = memory_path;
}

void os_ls_processes(){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "rb");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size];

    // Lee hasta 32 entradas
    size_t bytes_read;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        // Extrae los valores importantes
        unsigned char exists = (unsigned char)entry[0]; // Existe o no
        unsigned char process_name[15] = {0}; // 14 bytes + 1 para el terminador nulo
        strncpy(process_name, entry, 14); // Pasamos los 14 bytes del nombre
        int process_id = (unsigned char)entry[15]; // El id del proceso

        // Imprimimos en pantalla si el proceso existe
        if (exists == 0x01) {
            printf("%d %s\n", process_id, process_name);
        }
    }

    // Cierra el archivo
    fclose(file);

}

int os_exists(int process_id, char* file_name){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "rb");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size];

    // Lee hasta 32 entradas
    size_t bytes_read;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        int id = (unsigned char)entry[15]; // El id del proceso
        // Si el id es igual al id del proceso
        if(id == process_id){

            // Empieza a leer donde empieza el archivo
            int counter = 17;
            for(int i = 0; i < 10; i++){
                unsigned char archive_name[15] = {0};
                strncpy(archive_name, entry + counter, 14);
                // Verificamos si esta el archivo
                if(strcmp(archive_name, file_name) == 0){
                    fclose(file);
                    return 1;
                }

                // Cada 24 bytes mueve al inicio del siguiente archivo
                counter += 24;
            }
            fclose(file);
            return 0;
        }

    }
    fclose(file);
    return 0;

}

os_ls_files(int process_id){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "rb");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size];

    // Lee hasta 32 entradas
    size_t bytes_read;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        int id = (unsigned char)entry[15]; // El id del proceso
        if(id == process_id){

            int counter = 16;
            for(int i = 0; i < 10; i++){

                // Obtenemos los valores a imprimir

                // Nombre
                unsigned char archive_name[15] = {0};
                strncpy(archive_name, entry + counter + 1, 14);

                // Tamaño
                uint64_t archive_size = 0;
                for (int i = 0; i < 5; ++i) {
                    archive_size |= ((uint64_t)entry[counter + 15 + i] << (8 * i)); // Desplazamiento para dejarlo en little endian
                }

                // Dirrecion Virtual
                uint32_t virtual_direction = 0;
                for (int i = 0; i < 4; i++){
                    virtual_direction |= ((uint64_t)entry[counter + 20 + i] << (8 * i));
                }

                // VPN
                uint32_t VPN = (virtual_direction >> 15) & 0x1FFF; // Desplazamos 15 bits a la derecha (llegar a bit 32 - 15 = 17 mas signficativo) y aplicamos mascara de 13 bits (17 al 5)


                printf("%u %u %llu %s \n", VPN, archive_size, virtual_direction, archive_name);

                counter += 24;




            }
        }

    }

}