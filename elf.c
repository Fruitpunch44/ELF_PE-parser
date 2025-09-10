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
        fread(&header,sizeof(header),1,file);
        fseek(file,header.e_shoff+header.e_shstrndx*header.e_shentsize,SEEK_SET);
        ElfW(Shdr) section_headers;

        char *section_names=malloc(section_headers.sh_size);
        fseek(file,section_headers.sh_offset,SEEK_SET);
        fread(section_names,section_headers.sh_size,1,file);

        for(int idx =0; idx < header.e_shnum;++idx){
            char *name= " ";
            fseek(file,header.e_shoff+idx*sizeof(section_headers),SEEK_SET);
            fread(&section_headers,1,sizeof(section_headers),file);

            if(section_headers.sh_name);
            name=section_names+section_headers.sh_name;
            
            printf("%i %s",idx,name);

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
               fread(text,shdrs[i].sh_size,1,file);

                for (ssize_t j=0;j<shdrs[i].sh_size;j++){
                    printf("02x",text[j]);
                    if ((i+1) % 16 ==0){
                        printf(" ");
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
            print("Machine:%s",header.e_machine);
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


int main(int argc, char* argv[]){

    if(argc<2){
        printf("pls pass in the required arguments\n");
        exit(1);
    }
    char *file_name=argv[1];
    print_elf_headers(file_name);
    parse_text_section(file_name);
    return 0;
}