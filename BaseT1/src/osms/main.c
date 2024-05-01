#include "../osms_API/osms_API.h"

int main(int argc, char const *argv[])
{

    // montar la memoria
    os_mount((char *)argv[1]);
    os_ls_processes();
    os_exists(255, "BeachChant.mp3");
    os_ls_files(6);

}