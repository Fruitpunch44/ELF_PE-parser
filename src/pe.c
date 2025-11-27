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
    /*not all of em use something like cff explorer to get the other
    machine types*/
    switch(type){
        case 0x8664: return "AMD64";
        case 0x014c: return "Intel 386";
        case 0x01c0: return "ARM";
        case 0x0EBC: return "EFI BYTE CODE";
        case 0x0200: return "INTEL 64";
        default:     return "invalid PE TYPE";

    }
}


void print_FILE_HEADERS(IMAGE_NT_HEADERS* HEADER){
    fprintf(stdout,"----FILE_HEADERS-----\n");
    fprintf(stdout,"Machine : %s\n",machine_type(HEADER->FileHeader.Machine));
    fprintf(stdout,"Number of sections : 0x%x\n",HEADER->FileHeader.NumberOfSections);
    fprintf(stdout,"Pointer to symbol table : 0x%x\n",HEADER->FileHeader.PointerToSymbolTable);
    fprintf(stdout,"Number of symbols : 0x%x\n",HEADER->FileHeader.NumberOfSymbols);
    fprintf(stdout,"Size of optional header: 0x%x\n",HEADER->FileHeader.SizeOfOptionalHeader);
}

void parse_optional_heders(IMAGE_NT_HEADERS* HEADER){
    fprintf(stdout,"----OPTIONAL_HEADERS-----\n");
    fprintf(stdout,"Magic : %s\n",PE_type(HEADER->OptionalHeader.Magic));
    fprintf(stdout,"Address of entry point : 0x%x\n",HEADER->OptionalHeader.AddressOfEntryPoint);
    fprintf(stdout,"Image base : 0x%lx\n",HEADER->OptionalHeader.ImageBase);
    fprintf(stdout,"Section alignment : 0x%x\n",HEADER->OptionalHeader.SectionAlignment);
    fprintf(stdout,"File alignment : 0x%x\n",HEADER->OptionalHeader.FileAlignment);
    fprintf(stdout,"Size of image : 0x%x\n",HEADER->OptionalHeader.SizeOfImage);
    fprintf(stdout,"Size of headers : 0x%x\n",HEADER->OptionalHeader.SizeOfHeaders); 
}
void print_data(IMAGE_DOS_HEADER* DOS_HEADER,IMAGE_NT_HEADERS* NT_HEADER){
    parse_dos_header(DOS_HEADER);
    print_FILE_HEADERS(NT_HEADER);
    parse_optional_heders(NT_HEADER);

}
void parse_section_headers(IMAGE_NT_HEADERS *NT){
    IMAGE_SECTION_HEADER* section_names=(IMAGE_SECTION_HEADER*)(NT+1);
    fprintf(stdout,"----SECTIONS----\n");
    fprintf(stdout,"%-10s%-10s%-10s%-10s\n","NAME","RAWSIZE","VIRTUAL ADDRESS","CHARACTERISTICS");
    for(int i = 0; NT->FileHeader.NumberOfSections;i++){
        fprintf(stdout,"%-10s%-10#x%-10#x%-10#x\n",section_names[i].Name,section_names[i].SizeOfRawData,
                section_names[i].VirtualAddress,section_names[i].Characteristics);
        fprintf(stdout,"\n");
    }
}
void parse_dos_header(IMAGE_DOS_HEADER *Dos_header){
    fprintf(stdout,"----DOS_HEADER-----\n");
    fprintf(stdout,"e_magic : 0x%x\n",Dos_header->e_magic);
    fprintf(stdout,"e_cblp : 0x%x\n",Dos_header->e_cblp);
    fprintf(stdout,"e_cp : 0x%x\n",Dos_header->e_cp);
    fprintf(stdout,"e_crlc : 0x%x\n",Dos_header->e_crlc);
    fprintf(stdout,"e_cparhdr : 0x%x\n",Dos_header->e_cparhdr);
    fprintf(stdout,"e_minalloc : 0x%x\n",Dos_header->e_minalloc);
    fprintf(stdout,"e_maxalloc : 0x%x\n",Dos_header->e_maxalloc);
    fprintf(stdout,"e_ss : 0x%x\n",Dos_header->e_ss);
    fprintf(stdout,"e_sp : 0x%x\n",Dos_header->e_sp);
    fprintf(stdout,"e_lfarlc : 0x%x\n",Dos_header->e_lfarlc);
    fprintf(stdout,"e_ovno : 0x%x\n",Dos_header->e_ovno);
    fprintf(stdout,"e_lfanew : 0x%x\n",Dos_header->e_lfanew);
}

void *load_pe (char *PE_DATA,const char* FILE_NAME){
    IMAGE_DOS_HEADER* my_DOS_HEADER = (IMAGE_DOS_HEADER*)PE_DATA;
    IMAGE_NT_HEADERS* my_NT_HEADER = (IMAGE_NT_HEADERS*)(((char*)my_DOS_HEADER) + my_DOS_HEADER->e_lfanew);
    //
    if(my_DOS_HEADER->e_magic!=IMAGE_DOS_SIGNATURE){
        fprintf(stderr,"invalid Dos Signature\n");
        return NULL;
    }
    fprintf(stdout,"%s has a valid DOS Signature\n",FILE_NAME);
    print_data(my_DOS_HEADER,my_NT_HEADER);

    return NULL;

}
void load_exe_file(const char *file_path){
    FILE *exe_file;
    exe_file=fopen(file_path,"rb");
    if(!exe_file){
        fprintf(stderr,"unable to read file\n");
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