#pragma once
#include "../osms_File/Osms_File.h"
#include <stdint.h>
#include <stdio.h>

// funciones generales

void os_mount(char* memory_path);

void os_ls_processes();

int os_exists(int process_id, char* file_name);

void os_ls_files(int process_id);

void os_frame_bitmap(int PFN_i, int PFN_f);

// funciones procesos

int os_start_process(int process_id, char* process_name);

int os_finish_process(int process_id);

// // funciones archivos

OsmsFile* os_open(int process_id, char* file_name, char mode);

uint64_t os_write(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes);

uint64_t os_read(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes);

// uint64_t os_upload(int process_id, char* file_path);

uint64_t os_download(OsmsFile* file_desc, char* dest);

// void os_delete_file(int process_id, char* file_name); -- BONUS

void os_close(OsmsFile* file_desc);



// BORRAAAAAAAAAAAAAR

bool os_verify_space_subentry(int process_id, char *file_name);

void os_full_vd_array();

void os_edit_table(uint8_t *process_id_list, uint8_t *VPN_list, FILE* file);

int os_find_FPN(uint32_t process_id, uint32_t VPN, FILE* file);

void os_free();

void os_print();