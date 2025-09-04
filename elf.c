#include"my_elf.h"

void print_elf_headers(const char* elf_file){
    ElfW(Ehdr) header;

    FILE* file=fopen(elf_file,"rb");
    if(file){
        //read header
        fread(&header,sizeof(header),1,file);

        //elf file check
        if(memcmp(header.e_ident,ELFMAG,SELFMAG)==0){
            printf("%s is a valid elf file",elf_file);
            printf("ELF ID:%s\n",header.e_ident);
            printf("section header offset: %d\n",header.e_shoff);
            printf("entry point: %u",header.e_entry);
            printf("Elf header_size: %u",header.e_ehsize);
        }
        else{
            printf("%s is not a valid elf file",elf_file);
            exit(1);
        }
        fclose(file);
    }

}


int main(int argc, char* argv[]){

    if(argc<2){
        printf("pls pass in the required arguments\n");
        exit(1);
    }
    char *file_name=argv[1];
    print_elf_headers(file_name);
    return 0;
}