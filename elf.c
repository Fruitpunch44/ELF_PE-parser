#include<elf.h>
#include<stdio.h>
#include<string.h>


//check the system type if it's 64 bit
//if not use 32 bit

#if defined(__LP64__)
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif

void read_elf_headers(const char* elf_file){
    ElfW(Ehdr) header;

    FILE* file=fopen(elf_file,"rb");
    if(file){
        //read header
        fread(&header,sizeof(header),1,file);

        //elf file check
        if(memcmp(header.e_ident,ELFMAG,SELFMAG)==0){
            printf("%S is a valid elf file",elf_file);
        }
        fclose(file);
    }

}