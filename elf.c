#include"my_elf.h"

/* Return machine architecture name */
const char *elf_machine_name(uint16_t machine_num) {
    switch (machine_num) {
        case EM_386:    return "Intel 386";
        case EM_X86_64: return "AMD x86-64";
        case EM_ARM:    return "ARM";
        case EM_RISCV:  return "RISC-V";
        default:        return "Unknown machine type";
    }
}

/* Return OS/ABI name */
const char *elf_osabi_name(uint8_t osabi) {
    switch (osabi) {
        case ELFOSABI_SYSV:       return "UNIX - System V";
        case ELFOSABI_HPUX:       return "HP-UX";
        case ELFOSABI_NETBSD:     return "NetBSD";
        case ELFOSABI_LINUX:      return "Linux";
        case ELFOSABI_SOLARIS:    return "Solaris";
        case ELFOSABI_AIX:        return "AIX";
        case ELFOSABI_IRIX:       return "IRIX";
        case ELFOSABI_FREEBSD:    return "FreeBSD";
        case ELFOSABI_TRU64:      return "TRU64 UNIX";
        case ELFOSABI_MODESTO:    return "Novell Modesto";
        case ELFOSABI_OPENBSD:    return "OpenBSD";
        case ELFOSABI_ARM:        return "ARM";
        case ELFOSABI_STANDALONE: return "Standalone (embedded)";
        default:                  return "Unknown OS/ABI";
    }
}

/* Return ELF class (32/64-bit) */
const char *elf_class(uint16_t elf_class) {
    switch (elf_class) {
        case ELFCLASS32:   return "ELF32";
        case ELFCLASS64:   return "ELF64";
        case ELFCLASSNONE: return "Invalid class";
        default:           return "Not registered";
    }
}

/* Return data encoding (endianness) */
const char *elf_data_format(uint16_t elf_data) {
    switch (elf_data) {
        case ELFDATANONE: return "Unknown data format";
        case ELFDATA2LSB: return "Two's complement, little-endian";
        case ELFDATA2MSB: return "Two's complement, big-endian";
        default:          return "Not registered";
    }
}

/* Return ELF file type */
const char *elf_file_type(uint16_t elf_type) {
    switch (elf_type) {
        case ET_NONE: return "No file type";
        case ET_REL:  return "Relocatable file";
        case ET_EXEC: return "Executable file";
        case ET_DYN:  return "Shared object file";
        case ET_CORE: return "Core file";
        default:      return "Invalid file type";
    }
}

/* Return ELF version string */
const char *elf_version_name(uint16_t version) {
    switch (version) {
        case EV_NONE:    return "Invalid version";
        case EV_CURRENT: return "Current version";
        default:         return "Unknown version";
    }
}


void parse_section_table(const char *elf_file){
    ElfW(Ehdr) header;
    char *Section_names;
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


        Section_names=malloc(section_headers.sh_size);
        fseek(file,section_headers.sh_offset,SEEK_SET);
        fread(Section_names,section_headers.sh_size,1,file);

        for(int idx =0; idx < header.e_shnum;++idx){
            const char *name= "";
            fseek(file,header.e_shoff+idx*sizeof(section_headers),SEEK_SET);
            fread(&section_headers,sizeof(section_headers),1,file);

            name=Section_names+section_headers.sh_name;
            
            printf("%i %s\n",idx,name);

        }
        free(Section_names);
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

        //read all section headers
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
                free(text);
    
            }
        }
        free(shdrs);
        free(shstrtab);
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
            printf("%s is a valid elf file\n",elf_file);
            
            printf("ELF Magic: %02x %02x %02x %02x\n",
               header.e_ident[0], header.e_ident[1],
               header.e_ident[2], header.e_ident[3]);
            printf("Class: %s\n",elf_class(header.e_ident[4]));
            printf("Data: %s\n",elf_data_format(header.e_ident[5]));
            printf("OS/ABI: %s\n",elf_osabi_name(header.e_ident[7]));
            printf("Version: %s\n",elf_version_name(header.e_version));
            printf("Machine: %s\n",elf_machine_name(header.e_machine));
            printf("Type: %s\n",elf_file_type(header.e_type));
            printf("Start of section headers: %d\n",header.e_shoff);
            printf("Entry point: %#x\n",header.e_entry);
            printf("Size of section headers: %u\n",header.e_ehsize);
            printf("Number of section headers %u\n",header.e_shnum);
            printf("Flags: %#lx\n",header.e_flags);
        

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

int main(int argc,char *argv []){
    if(argc<2){
        fprintf(stderr,"please pass in the right number of args");
        exit(EXIT_FAILURE);
    }
    const char *file_name=argv[1];
    print_elf_headers(file_name);
    return 0;

}