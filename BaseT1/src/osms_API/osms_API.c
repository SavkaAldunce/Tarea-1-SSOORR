#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> //para strcmp
#include <stdbool.h> // bool, true, false
#include <stdint.h>
#include "../osms_File/Osms_File_list.h"
#include "../osms_File/Osms_File.h"

char* path;
FilesList *list;
int list_len = 0;
uint32_t vd_list[320];


// Funciones hechas por nosotros :D

// Verificar si hay espacio para el archivo y realizar cambios en la memoria pertinentes
bool os_verify_space_subentry(int process_id, char *file_name){

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "r+b");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size];

    // Lee hasta 32 entradas
    size_t bytes_read;
    size_t total_bytes_read = 0;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        } else {
            total_bytes_read = total_bytes_read + bytes_read; // Para saber a donde mover el puntero nuevamente en el archivo
        }

        int id = (unsigned char)entry[15]; // El id del proceso
        // Si el id es igual al id del proceso
        if(id == process_id){

            // Empieza a leer donde empieza el archivo
            int counter = 16;
            for(int i = 0; i < 10; i++){
                unsigned char exists = (unsigned char)entry[0];
                if(exists == 0x00){

                    // Validez
                    entry[0] = 1;

                    // Tamaño
                    for(int i = 15; i < 20; i++){
                        entry[i] = 0;
                    }

                    // Nombre
                    char array[14];  // Array de 14 bytes

                    // Verifica si la cadena tiene al menos 14 caracteres
                    if (strlen(file_name) >= 14) {
                        memcpy(array, file_name, 14);  // Copia los primeros 14 bytes
                    } else {
                        // Si nombre es más corto que 14 caracteres, copia lo que hay y rellena con ceros
                        memcpy(array, file_name, strlen(file_name));
                        memset(array + strlen(file_name), 0, 14 - strlen(file_name));  // Rellena el resto con ceros
                    }

                    for(int i = 0; i < 14; i++){
                        entry[i + 1] = array[i];
                    }

                    // Direccion virtual
                    uint32_t vd;
                    while(true){
                        vd = rand() & 0x03FFFF; // 0x03FFFF es una máscara que permite solo los primeros 22 bits (los 4 más significativos son cero)
                        bool vd_already_exists = false;
                        for(int i = 0; i < 320; i++){
                            if(vd == vd_list[i]){
                                vd_already_exists = true;
                                break;
                            }
                        }
                        if(vd_already_exists == false){
                            break;
                        }
                    }
                    // Agregamos direccion a nuestra lista
                    for(int i = 0; i < 320; i++){
                        if((int)vd_list[i] == 0){
                            vd_list[i] = vd;
                        }
                    }
                    uint32_t VPN = (vd >> 15) & 0x1FFF; // Extraemos el VPN
                    uint8_t vpn_bits[10]; // Arreglo para almacenar cada uno de los 10 bits
                    // Extracción y almacenamiento de los 10 bits menos significativos
                    for (int i = 0; i < 10; i++) {
                        vpn_bits[i] = (VPN >> i) & 1; // Desplaza la variable 'var' i posiciones y aplica máscara para obtener el bit
                    }
                    uint32_t process_id_uint = (u_int32_t)process_id;
                    uint8_t process_bits[10]; // Arreglo para almacenar cada uno de los 10 bits
                    // Extracción y almacenamiento de los 10 bits menos significativos
                    for (int i = 0; i < 13; i++) {
                        process_bits[i] = (process_id >> i) & 1; // Desplaza la variable 'var' i posiciones y aplica máscara para obtener el bit
                    }
                    entry[20] = (vd >> 24) & 0xFF; // Extrae el byte más significativo
                    entry[21] = (vd >> 16) & 0xFF; // Extrae el segundo byte más significativo
                    entry[22] = (vd >> 8) & 0xFF;  // Extrae el tercer byte más significativo
                    entry[23] = vd & 0xFF; 

                    // Cambiamos tabla invertida
                    fseek(file, -total_bytes_read, SEEK_CUR);
                    os_edit_table(process_bits, vpn_bits, file);
                    fseek(file, total_bytes_read, SEEK_SET);

                    // Mover el puntero del archivo a la posición inicial de la entrada actual
                    fseek(file, -((long)entry_size), SEEK_CUR);

                    // Escribir la entrada modificada de vuelta al archivo
                    fwrite(entry, 1, entry_size, file);

                    // Asegurarse de que los cambios se guarden en el disco
                    fflush(file);

                    fclose(file);

                    return true;

                }
                // Cada 24 bytes mueve al inicio del siguiente archivo
                counter += 24;
            }
            fclose(file);
            return false;
        }

    }
    fclose(file);
    return false;

}

void os_full_vd_array(){
    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "rb");

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size];

    // Lee hasta 32 entradas
    size_t bytes_read;
    int counter_array = 0;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        }

        int counter = 16;
        for(int i = 0; i < 10; i++){
            // Direcion Virtual
            uint32_t virtual_direction = 0;
            for (int i = 0; i < 4; i++){
                virtual_direction |= ((uint64_t)entry[counter + 20 + i] << (8 * i));
            }
            vd_list[counter_array] = virtual_direction;
            counter_array = counter_array + 1;
            counter += 24;
            }

    }
    for(int i = 0; i < 320; i++){
        printf("elemento %d: 0x%X\n", i, vd_list[i]);
    }
}

void os_edit_table(uint8_t *process_id_list, uint8_t *VPN_list, FILE* file){

    // Nos movemos hasta la parte del archivo que queremos
    size_t entry_size = 3;
    unsigned char entry[entry_size];

    fseek(file, 8192, SEEK_SET);

    size_t bytes_read;
    for(int i = 0; i < 65536; i++){
        bytes_read = fread(entry, 1, entry_size, file);

        // Extraer el primer bit
        int exist = (entry[0] >> 7) & 1;  // Desplaza 7 bits a la derecha y aplica máscara para el último bit
        int bit_counter = 0;
        int pointer_process_id = 0;
        int pointer_vpn = 0;
        if(exist == 0){
            for (int i = 0; i < 3; i++) {
            // Recorrer cada bit en un byte específico
            for (int j = 0; j < 8; j++) {
                bit_counter += 1;

                if(bit_counter == 1){
                    entry[i] |= (1 << j);
                }
                else if(bit_counter >= 2 && bit_counter <= 11 && (int)process_id_list[pointer_process_id] == 1){
                    entry[i] |= (1 << j);
                    pointer_process_id = pointer_process_id + 1;
                }
                else if(bit_counter >= 2 && bit_counter <= 11 && (int)process_id_list[pointer_process_id] == 0){
                    entry[i] &= ~(1 << j);
                    pointer_process_id = pointer_process_id + 1;
                }
                else if(bit_counter >= 12 && (int)VPN_list[pointer_vpn] == 1){
                    entry[i] |= (1 << j);
                    pointer_vpn = pointer_vpn + 1;
                }
                else if(bit_counter >= 12 && (int)VPN_list[pointer_vpn] == 0){
                    entry[i] &= ~(1 << j);
                    pointer_vpn = pointer_vpn + 1;
                }

            }
        }
        // Mover el puntero del archivo a la posición inicial de la entrada actual
        fseek(file, -((long)entry_size), SEEK_CUR);

        // Escribir la entrada modificada de vuelta al archivo
        fwrite(entry, 1, entry_size, file);

        // Asegurarse de que los cambios se guarden en el disco
        fflush(file);
        }
    }
}

int os_find_FPN(uint32_t process_id, uint32_t VPN, FILE* file){

    // Nos movemos hasta la parte del archivo que queremos
    size_t entry_size = 3;
    unsigned char entry[entry_size];

    fseek(file, 8192, SEEK_SET);

    for(int i = 0; i < 65536; i++){
        fread(entry, 1, entry_size, file);

        // Extraer el primer bit
        int exist = (entry[0] >> 7) & 1;  // Desplaza 7 bits a la derecha y aplica máscara para el último bit
        // Extraer los siguientes 10 bits
        uint32_t id = ((entry[0] & 0x7F) << 3) | (entry[1] >> 5);  // Extrae los últimos 7 bits del primer byte y los primeros 3 bits del segundo byte
        // Extraer los últimos 13 bits
        uint32_t vpn = ((entry[1] & 0x1F) << 8) | entry[2];  // Extrae los últimos 5 bits del segundo byte y todos los bits del tercer byte

        if(id == process_id && vpn == VPN){
            return i + 1; //Encontramos la entrada
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
        int counter = 16;
        for(int i = 0; i < 10; i++){
            // Existe
            unsigned char exists = (unsigned char)entry[counter];
            // Dirrecion Virtual
            uint32_t virtual_direction = 0;
            for (int i = 0; i < 4; i++){
                virtual_direction |= ((uint64_t)entry[counter + 20 + i] << (8 * i));
            }
            printf("existe: %d, direccion virtual: 0x%X  \n", exists, virtual_direction);
            counter += 24;
            }

    }
    entry_size = 3;
    for(int i = 0; i < 65536; i++){
        bytes_read = fread(entry, 1, entry_size, file);

        // Extraer el primer bit
        int exist = (entry[0] >> 7) & 1;  // Desplaza 7 bits a la derecha y aplica máscara para el último bit

        // Extraer los siguientes 10 bits
        int process_id = ((entry[0] & 0x7F) << 3) | (entry[1] >> 5);  // Extrae los últimos 7 bits del primer byte y los primeros 3 bits del segundo byte

        // Extraer los últimos 13 bits
        int vpn = ((entry[1] & 0x1F) << 8) | entry[2];  // Extrae los últimos 5 bits del segundo byte y todos los bits del tercer byte

        // Imprimir los resultados
        if(exist == 1){
            printf("Existe: %d, id proceso: %d, VPN: %d\n", exist, process_id, vpn);
        }
    }

    fclose(file);

}

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
        unsigned char byte = (unsigned char)process_id;
        int new_process_id = (unsigned char)byte;

        // Si el id es igual al id del proceso
        if(id == new_process_id){

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
        unsigned char byte = (unsigned char)process_id;
        int new_process_id = (unsigned char)byte;

        if(id == new_process_id){

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

OsmsFile* os_open(int process_id, char* file_name, char mode){

    unsigned char byte = (unsigned char)process_id;
    int new_process_id = (unsigned char)byte;

    if(mode == "r"){
        // Verificamos si archivo existe y si esta en la lista de archivos (si struct ya esta creado)
        if(os_exists(new_process_id, file_name) == 0 & file_exists(new_process_id, file_name, list, list_len) == false){
            return NULL;
        }
        else if(os_exists(new_process_id, file_name) == 1 & file_exists(new_process_id, file_name, list, list_len) == false){
            OsmsFile file = file_create(new_process_id, file_name);
            if(list_len == 0){
                list = fileslist_init(file);
            }
            else {
                fileslist_append(list, file);
            }
            list_len = list_len + 1;
            return(fileslist_at_index(list, list_len - 1)); // retornamos struct
        }
        else if(os_exists(new_process_id, file_name) == 1 & file_exists(new_process_id, file_name, list, list_len) == true) {
            int id = id_file(new_process_id, file_name, list, list_len);
            return(fileslist_at_index(list, id - 1));
        }
    }
    else if(mode == "w"){
        // Verificamos que archivo exista
        if(os_exists(new_process_id, file_name) == 0 & file_exists(new_process_id, file_name, list, list_len) == false){
            // Vemos si hay espacio para el archivo
            bool available_space = os_verify_space_subentry(new_process_id, file_name);
            if(available_space == true){
                OsmsFile file = file_create(new_process_id, file_name);
                if(list_len == 0){
                    list = fileslist_init(file);
                }
                else {
                    fileslist_append(list, file);
                }
                list_len = list_len + 1;
                return(fileslist_at_index(list, list_len - 1)); // Retornamos el struct
            }
            else {
                return NULL;
            }
        }
    }
}

uint64_t os_write(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes) {

    FILE *file;

    // Abre el archivo en modo lectura binaria
    file = fopen(path, "r+b");

    // FPN
    int FPN;


    // Obtenemos el frame donde debemos escribir //

    // Definimos los bytes que contendra cada entrada y donde se almacenara la entrada
    size_t entry_size = 256;
    unsigned char entry[entry_size];

    // Lee hasta 32 entradas
    size_t bytes_read;
    size_t total_bytes_read = 0;
    for (int i = 0; i < 32; ++i) {
        // Lee un bloque de 256 bytes
        bytes_read = fread(entry, 1, entry_size, file);

        // Si no se leen más datos, salir del bucle
        if (bytes_read == 0) {
            break;
        } else {
            total_bytes_read = total_bytes_read + bytes_read;
        }

        int process_id = (unsigned char)entry[15]; // El id del proceso
        if(process_id == file_desc->process_id){
            int counter = 16;
            for(int i = 0; i < 10; i++){

                // Nombre del archivo
                unsigned char archive_name[15] = {0};
                strncpy(archive_name, entry + counter + 1, 14);

                if(strcmp(archive_name, file_desc->file_name) == 0){
                    // Direcion Virtual
                    uint32_t virtual_direction = 0;
                    for (int i = 0; i < 4; i++){
                        virtual_direction |= ((uint64_t)entry[counter + 20 + i] << (8 * i));
                    }
                    uint32_t VPN = (virtual_direction >> 15) & 0x1FFF; // Obtenemos el VPN
                    fseek(file, -total_bytes_read, SEEK_CUR);
                    FPN = os_find_FPN((uint32_t)process_id, VPN, file);
                    fseek(file, 212992, SEEK_SET); //Seteamos para leer desde los frames
                    break; // Obtuvimos el FPN

                }
                counter += 24;
            }
        }

    }

    // Escribimos en el archivo

    fseek(file, 32768*FPN, SEEK_CUR); //Seteamos para leer desde los frames
    if(n_bytes <= 32768 - file_desc->current_position){
        fseek(file, file_desc->current_position, SEEK_CUR); // Movemos donde se tiene que empezar a escribir
        fwrite(buffer, 1, n_bytes, file);
        file_desc->current_position = file_desc->current_position + n_bytes;
        fclose(file);
        return n_bytes;
    } else{
        int n_bytes_to_write = 32768 - file_desc->current_position; // Cuantos bytes vamos a escribir
        fseek(file, file_desc->current_position, SEEK_CUR); // Movemos donde se tiene que empezar a escribir
        fwrite(buffer, 1, n_bytes_to_write, file);
        file_desc->current_position = file_desc->current_position + n_bytes_to_write;
        fclose(file);
        return n_bytes_to_write;
    }
}

uint64_t os_read(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes){
    
}

void os_close(OsmsFile* file_desc){
    int id = file_desc->file_id;
    for(int i = 0; i < list_len; i++){
        OsmsFile* file = fileslist_at_index(list, i);
        if(file->file_id == id){
            list = fileslist_delete(list, id); // Lo sacamos de la lista
            free(file); // Liberamos la memoria
            list_len = list_len - 1;
        }
    }

}