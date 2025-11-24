#include "PE.h"

const char *PE_type(DWORD type){
    switch(type){
        case 0X10B : return "PE32";
        case 0x20B : return "PE64";
        case 0x107 : return "ROM";
        default: return "INVALID PE TYPE";
    }
}

const char *machine_type(WORD type){
    switch(type){
        case 0x8664: return "AMD64";
        case 0x014c: return "Intel 386";
        case 0x01c0: return "ARM";
        case 0x0EBC: return "EFI BYTE CODE";
        case 0x0200: return "INTEL 64";
        default:     return "invalid PE TYPE";

    }
}
void print_dos();
void print_image();
void parse_file_headers();
void parse_optional_heders();
void parse_section_headers();
void parse_imports();


void *load_pe (char *PE_DATA,const char* FILE_NAME){
    IMAGE_DOS_HEADER* my_DOS_HEADER = (IMAGE_DOS_HEADER*)PE_DATA;
    IMAGE_NT_HEADERS* my_NT_HEADER = (IMAGE_NT_HEADERS*)(((char*)my_DOS_HEADER) + my_DOS_HEADER->e_lfanew);
    //
    if(my_DOS_HEADER->e_magic!=IMAGE_DOS_SIGNATURE){
        fprintf(stderr,"invalid Dos Signature ");
        return NULL;
    }
    fprintf(stdout,"%s has a valid DOS Signature",FILE_NAME);
    print_data(my_DOS_HEADER,my_NT_HEADER);
    return ;
}
void load_exe_file(const char *file_path){
    FILE *exe_file;
    exe_file=fopen(file_path,"rb");
    if(!exe_file){
        fprintf(stderr,"unable to read file");
        exit(EXIT_FAILURE);
    }
    fseek(exe_file,0L,SEEK_END);
    long int file_size = ftell(exe_file);
    fseek(exe_file,0L,SEEK_SET);

    char *exe_data = malloc(sizeof(exe_file));
    if(!exe_data){
        fprintf(stderr,"unable to allocate memory for exe data");
        fclose(exe_file);
        exit(EXIT_FAILURE);
    }
    if(fread(exe_data,1,file_size,exe_file) != file_size){
        fprintf(stderr,"unable to read exe data");
        free(exe_data);
        fclose(exe_file);
        exit(EXIT_FAILURE);
    }
    //load pe file
    fprintf(stdout,"LOADING PE FILE");
    void *start_address = load_pe(exe_data,file_path);
    if(start_address){
        ((void (*)(void)) start_address)();
    }

}