#include "../osms_API/osms_API.h"

int main(int argc, char const *argv[])
{

    // montar la memoria
    os_mount((char *)argv[1]);
    os_ls_processes();
    os_exists(255, "BeachChant.mp3");
    os_ls_files(6);
    os_frame_bitmap(29642, 30998);
    os_start_process(1, "Hola");
    os_finish_process(1);
    
    // Borraaaaaaaar
    os_print();
    os_full_vd_array();

}