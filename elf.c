#include"my_elf.h"



void parse_section_table(const char* elf_file){
    ElfW(Ehdr) header;
    const char *Section_names;
    FILE *file=fopen(elf_file,"rb");
    if(!file){
        perror("an error occured reading the file");
        exit(1);
    }
    else{
        //read elf header first
        fread(&header,sizeof(header),1,file);
        //move to the section header string table
        fseek(file,header.e_shoff+header.e_shstrndx*header.e_shentsize,SEEK_SET);

        //read section headers
        ElfW(Shdr) section_headers;
        fread(&section_headers, sizeof(section_headers), 1, file);


        char *section_names=malloc(section_headers.sh_size);
        fseek(file,section_headers.sh_offset,SEEK_SET);
        fread(section_names,1,section_headers.sh_size,file);

        for(int idx =0; idx < header.e_shnum;++idx){
            const char *name= "";
            fseek(file,header.e_shoff+idx*sizeof(section_headers),SEEK_SET);
            fread(&section_headers,1,sizeof(section_headers),file);

            name=section_names+section_headers.sh_name;
            
            printf("%i %s\n",idx,name);

        }
    }
    
}

void parse_text_section(const char* elf_file){
    ElfW(Ehdr) header;
    FILE *file=fopen(elf_file,"rb");
    if(!file){
        perror("could not read the elf file");
        exit(1);
    
    }
    if(file){
        //read header first
        fread(&header,sizeof(header),1,file);
        fseek(file,header.e_shoff,SEEK_SET);

        ElfW(Shdr) *shdrs = malloc(header.e_shnum * sizeof(ElfW(Shdr)));
        fread(shdrs, sizeof(ElfW(Shdr)), header.e_shnum, file);


        ElfW(Shdr) shstr = shdrs[header.e_shstrndx];
        fseek(file, shstr.sh_offset, SEEK_SET);
        char *shstrtab =malloc(shstr.sh_size);
        fread(shstrtab, shstr.sh_size, 1, file);

    

        for (int i = 0; i < header.e_shnum; i++){
            const char *name=shstrtab+ shdrs[i].sh_name;
            if (strcmp(name,".text")==0){
                printf("Section .text at offset %lu, size %lu\n",
               (unsigned long)shdrs[i].sh_offset,
               (unsigned long)shdrs[i].sh_size);
               fseek(file,shdrs[i].sh_offset,SEEK_SET);

               unsigned char *text=malloc(shdrs[i].sh_size);
               if(!text){
                perror("error in allocating memory");
                exit(1);
               }
               fread(text,shdrs[i].sh_size,1,file);
               printf("Machine Code \n");
                for (ssize_t j=0;j<shdrs[i].sh_size;j++){
                    printf("%02x ",text[j]);
                    if ((j+1) % 16 ==0){
                        printf("\n");
                    }
                }
            }
        }
    } 
}

void print_elf_headers(const char* elf_file){
    ElfW(Ehdr) header;
    FILE* file=fopen(elf_file,"rb");
    if(!file){
        perror("unable to read file");
        exit(1);
    }

    if(file){
        //read header
        fread(&header,sizeof(header),1,file);
        

        //elf file check
        if(memcmp(header.e_ident,ELFMAG,SELFMAG)==0){
            printf("%s is a valid elf file",elf_file);
            
            printf("ELF Magic: %02x %02x %02x %02x\n",
               header.e_ident[0], header.e_ident[1],
               header.e_ident[2], header.e_ident[3]);

            printf("Version:%d\n",header.e_version);
            printf("Machine:%u",header.e_machine);
            printf("section header offset: %d\n",header.e_shoff);
            printf("entry point: %#x\n",header.e_entry);
            printf("Elf header_size: %u\n",header.e_ehsize);


            //add more things
            //read the docs
        }
        else{
            printf("%s is not a valid elf file",elf_file);
            exit(1);
        }
        fclose(file);
    }

}

