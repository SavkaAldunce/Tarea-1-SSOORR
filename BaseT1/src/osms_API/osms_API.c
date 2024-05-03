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


                printf("0x%X %u 0x%X %s \n", VPN, archive_size, virtual_direction, archive_name);

                counter += 24;




            }
        }

    }

    fclose(file);

}


void os_frame_bitmap(int PFN_i, int PFN_f){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "rb");

    // Contadores de Frames usados y libres
    int count_used = 0;
    int count_free = 0;
    int count_bits = 0;

    fseek(file, 204800, SEEK_SET);

    // Definimos los bytes que contendra cada entrada (1 queremos contar bits) y donde se almacenara la entrada
    size_t entry_size = 1;
    unsigned char entry[1];

    size_t byte_read;
    for(int i = 0; i < 8192; i++){

        byte_read = fread(entry, 1, entry_size, file);
        for (int i = 7; i >= 0; i--) {
            // Aislamos el bit
            int bit = (byte_read >> i) & 1;
            count_bits = count_bits + 1;
            if(count_bits >= PFN_i & count_bits <= PFN_f){
                if(bit == 1){
                    count_used = count_used + 1;
                } else {
                    count_free = count_free + 1;
                }
                printf("%d", bit);
            }
            if(count_bits == PFN_f){
                break; // Para eficiencia
            }

    }

    }

    printf("\n");
    printf("USADOS %d\n", count_used);
    printf("LIBRES %d\n", count_free);
    fclose(file);
}

int os_start_process(int process_id, char* process_name){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "r+b");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size + 1];

    // Lee hasta 32 entradas para verificar si id ya existe
    size_t bytes_read;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        int exists = (unsigned char)entry[0]; // Existe o no
        int id = (unsigned char)entry[15]; // El id del proceso
        // Cambiamos valor de process_id a su valor representado en 1 byte
        unsigned char byte = (unsigned char)process_id;
        int new_process_id = (unsigned char)byte;
        unsigned char process_name[15] = {0}; // 14 bytes + 1 para el terminador nulo
        strncpy(process_name, entry, 14); // Pasamos los 14 bytes del nombre
        if(id == new_process_id){
            fclose(file);
            return -1;
        }
    }

    rewind(file);
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        int id = (unsigned char)entry[15]; // El id del proceso
        if(id == 0){

            entry[0] = 1;
            entry[15] = process_id;

            char array[14];  // Array de 14 bytes

            // Verifica si la cadena tiene al menos 14 caracteres
            if (strlen(process_name) >= 14) {
                memcpy(array, process_name, 14);  // Copia los primeros 14 bytes
            } else {
                // Si nombre es más corto que 14 caracteres, copia lo que hay y rellena con ceros
                memcpy(array, process_name, strlen(process_name));
                memset(array + strlen(process_name), 0, 14 - strlen(process_name));  // Rellena el resto con ceros
            }

            for(int i = 0; i < 14; i++){
                entry[i + 1] = array[i];
            }

            // Mover el puntero del archivo a la posición inicial de la entrada actual
            fseek(file, -((long)entry_size), SEEK_CUR);

            // Escribir la entrada modificada de vuelta al archivo
            fwrite(entry, 1, entry_size, file);

            // Asegurarse de que los cambios se guarden en el disco
            fflush(file);

            break;

        }

    }

    fclose(file);
    return 0;
}

int os_finish_process(int process_id){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "r+b");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size + 1];

    // Lee hasta 32 entradas para verificar si id ya existe
    size_t bytes_read;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        int id = (unsigned char)entry[15]; // El id del proceso
        // Cambiamos valor de process_id a su valor representado en 1 byte
        unsigned char byte = (unsigned char)process_id;
        int new_process_id = (unsigned char)byte;

        if(id == new_process_id){

            // Seteamos todos los bytes de esa entrada en 0
            for(int i = 0; i < 256; i++){
                entry[i] = 0;
            }

            // Mover el puntero del archivo a la posición inicial de la entrada actual
            fseek(file, -((long)entry_size), SEEK_CUR);

            // Escribir la entrada modificada de vuelta al archivo
            fwrite(entry, 1, entry_size, file);

            // Asegurarse de que los cambios se guarden en el disco
            fflush(file);

            break;
        }

    }
}

void os_print(){

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
        printf("Existe: %d, id: %d, nombre: %s\n", exists, process_id, process_name);

    }

    fclose(file);

}